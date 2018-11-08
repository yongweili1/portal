#ifndef __mcsf_communication_proxy_impl_hpp__
#define __mcsf_communication_proxy_impl_hpp__
#endif

#include "../include/McsfNetBase/mcsf_communication_proxy.h"

#include "icom_proxy.h"
#include <stdexcept>

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include "client.hpp"
#include "server.hpp"
#include "mcsf_blobdata_interface.h"

#include "id_def.h"
#include "send_data_cmd_handler.hpp"
#include "mcsf_share_memory_sender.h"
#include "mcsf_socket_sender.h"
#include "async_blob_data_callback_handler.hpp"

#define force_use_socket 0

namespace Mcsf {

    class CommunicationProxy::Implement
    {
    public:
        struct ImplEntry
        {
            std::string name;
            int ioThrNum;
            int handleThreNum;
            int ioReactorPriority;
            std::string dispatcherAddr;
            std::string listenPort;
        };

        explicit Implement(const ImplEntry & entry);

        ~Implement();

        int AsyncSendCommand(const CommandContext* pCommandContext);

        int SyncSendDataEx(const SendDataContextEx& pSendContext, std::string& sSerializeObject);

        ISyncResult* SyncSendDataEx(const SendDataContextEx& pSendContext);

        int AsyncSendDataEx(const SendDataContextEx& pSendContext);


        int RegisterCommandHandler(int iCommandId, ICommandHandler* pCommandHandler);


        int RegisterCommandHandlerEx(int iCommandId, boost::shared_ptr<ICommandHandlerEx> pCommandHandler);


        void UnRegisterCommandHandler(int iCommandId);

        void UnRegisterAllCommandHandlers(void);


        int RegisterDataHandlerEx(boost::shared_ptr<IDataHandlerEx> pSendDataHandler);

        void UnRegisterDataHandler(void);


        int SendEvent(const std::string& sSender, int iChannelId, int iEventId, const std::string& sEvent);


        int BroadcastEvent(const std::string& sSender, int iChannelId, int iEventId, const std::string& sEvent);


        int RegisterEventHandler(int iChannelId, int iEventId, IEventHandler* pEventHandler);


        void UnRegisterEventHandler(int iChannelId, int iEventId);

        void UnRegisterAllEventHandlers();


        void UnRegisterAllHandlers();

        int SyncSendCommand(const CommandContext* pCommandContext, std::string& sSerializeObject);

        ISyncResult* SyncSendCommand(const CommandContext* pCommandContext);

        std::string GetName() const;

        std::string GetSystemDispatcherAddress(void) const;

        int SendSystemEvent(const std::string& sSender, int iChannelId, int iEventId, const std::string& sEvent);

        int BroadcastSystemEvent(const std::string& sSender, int iChannelId, int iEventId, const std::string& sEvent);

        int CheckCastName(const std::string& sName, int iWaitTime);
      
        std::string getIPsPort() { return m_pServer->getIPsPort(); }

        std::string GetMacs() { return m_pServer->getMacs(); }

    private:
        netbasecore::connection_ptr build_connection(const std::string &name, int &errorNum, int timout = 0);

        int sync_send_command(netbasecore::connection_ptr conn, const Mcsf::CommandContext &command, std::string &dataOut);
        int get_uid();
        bool register_to_dispatcher();

        void stop();

        int subscribe_event(const std::set<int> &eventIDs);

        boost::shared_ptr<IDataSender> create_sender(const std::string &sPeerIP);

        int send_event(int type, int iChannelId, int iEventId, const std::string &sEvent);

    private:
        netbasecore::io_service_pool m_svrPool;
        netbasecore::callback_handle m_callHandlers;
        netbasecore::connection_manager m_connectionMngr;
        boost::shared_ptr<netbasecore::client> m_pClient;
        boost::shared_ptr<netbasecore::server> m_pServer;
        boost::mutex m_mxConnect;
        std::set<int> m_setSubcribIDs;
        boost::mutex m_uidmx;
        int m_uid;
    };

    CommunicationProxy::Implement::Implement(const ImplEntry & entry)
        : m_callHandlers(entry.name)
        , m_uid(0)
        , m_svrPool(entry.ioThrNum, entry.handleThreNum)
        , m_connectionMngr()
        , m_pClient(new netbasecore::client(m_svrPool, m_callHandlers, m_connectionMngr))
        , m_pServer(new netbasecore::server(entry.listenPort, m_svrPool, m_connectionMngr, m_callHandlers))
    {
        m_connectionMngr.set_reconnect_handler(boost::bind(
            &netbasecore::client::handle_reconnect_dispatcher, m_pClient, _1));

        m_pClient->set_register_handler(boost::bind(&Implement::register_to_dispatcher, this));

        netbasecore::util::host_port_t hp(entry.dispatcherAddr);
        if (!hp.parsed_ok) {
            throw std::runtime_error("exception on parse dispatcherAddr:" + entry.dispatcherAddr);
        }
        else {
            if (m_pClient->check_cast_dispatcher(hp.host, hp.port)) {
                if (!register_to_dispatcher()) {
                    throw std::runtime_error("register_to_dispatcher failed!");
                }
            }
            else
            {
                m_pClient->handle_reconnect_dispatcher(nullptr);
            }
        }
    }

    void CommunicationProxy::Implement::stop()
    {
        m_svrPool.stop();
        NTB_LOG_INFO(m_callHandlers.getMyName() << " m_svrPool::stop end.");
        m_pServer->stop();
        m_pClient->stop();

        m_connectionMngr.stop_all();
        m_svrPool.join_all();
        NTB_LOG_INFO(m_callHandlers.getMyName() << " Implement::stop end.");
    }

    CommunicationProxy::Implement::~Implement()
    {
        stop();
    }

    std::string CommunicationProxy::Implement::GetName() const {
        return m_callHandlers.getMyName();
    }

    bool CommunicationProxy::Implement::register_to_dispatcher() {
        netbasecore::connection_ptr c = m_connectionMngr.get_connection(dispatcher_name);
        if (c) {
            Mcsf::CommandContext cx;
            cx.iCommandId = register_id;
            cx.sReceiver = dispatcher_name;
            McsfRegisterContaineeRequest regQuest;
            regQuest.set_name(netbasecore::server::getMacs());
            regQuest.set_ip_addr(m_pServer->getIPsPort());
            regQuest.SerializeToString(&cx.sSerializeObject);
            cx.iWaitTime = 1000;
            std::string reply;
            int erro = sync_send_command(c, cx, reply);
            if (0 != erro) {
                NTB_LOG_ERROR(GetName() << " register_id failed.erro=" << erro);
                return false;
            }
            erro = subscribe_event(m_setSubcribIDs);
            if (0 != erro) {
                NTB_LOG_ERROR(GetName() << " subscribe_event failed.erro=" << erro);
                return false;
            }

            return atoi(reply.c_str());
        }
        else {
            NTB_LOG_ERROR("no connection with dispatcher.");
            return false;
        }
    }

    int CommunicationProxy::Implement::get_uid() {
        boost::mutex::scoped_lock l(m_uidmx);
        return ++m_uid;
    }

    netbasecore::connection_ptr CommunicationProxy::Implement::build_connection(const std::string &name, int &errorNum, int timout)
    {
        netbasecore::connection_ptr c = m_connectionMngr.get_connection(name);
        if (c)
            return c;

        c = m_connectionMngr.get_connection(dispatcher_name);
        if (!c) {
            errorNum = Mcsf::McsfUndefineError;
            return nullptr;
        }

        Mcsf::CommandContext cx;
        cx.iCommandId = find_id;
        cx.iWaitTime = 1000;
        cx.sReceiver = dispatcher_name;
        cx.sSerializeObject = name;
        std::string reply;
        if (0 != sync_send_command(c, cx, reply)) {
            errorNum = Mcsf::McsfQueryNameCommandFailed;
            return nullptr;
        }

        if (reply.empty()) {
            errorNum = Mcsf::McsfContaineeNotRegistedInDispatcher;
            return nullptr;
        }

        boost::mutex::scoped_lock lk(m_mxConnect);
        c = m_connectionMngr.get_connection(name);
        if (c)
            return c;
        netbasecore::util::host_port_t hp(reply);
        if (!m_pClient->Connect(hp.host, hp.port, timout)) {
            errorNum = Mcsf::McsfSocketConnectFailed;
            return nullptr;
        }
        NTB_LOG_INFO("connect success." << hp.to_string());

        return m_connectionMngr.get_connection(name);
    }

    int CommunicationProxy::Implement::sync_send_command(netbasecore::connection_ptr conn,
        const Mcsf::CommandContext &command, std::string &dataOut)
    {
        netbasecore::util::RpcMsgPackagePtr p(
            new netbasecore::util::RpcMsgPackage(netbasecore::util::msg_type_cmd));
        p->m_header.uid = get_uid();
        p->m_header.id = command.iCommandId;
        p->setData(command.sSerializeObject);

        netbasecore::pending_call_ptr call(
            new netbasecore::pending_call(command.sReceiver, m_svrPool.get_reply_handle_service(), true,
                command.pCommandCallback, command.iWaitTime));
        m_callHandlers.register_pending_call(p->m_header.uid, call);

        if (echo_cmd_id != p->m_header.id)
            NTB_LOG_INFO(GetName() << "=>" << command.sReceiver << " c=" << command.iCommandId << " u=" << p->m_header.uid << " l=" << p->m_header.uDataLen);

        if (!conn->push_message(p))
        {
            NTB_LOG_INFO("push message error!");
            return Mcsf::McsfSocketConnectFailed;
        }

        if (call->wait())
        {
            m_callHandlers.unregister_pending_call(p->m_header.uid);

            netbasecore::util::RpcMsgPackagePtr reply = call->getReplyResult();
            if (reply) {
                if (reply->m_header.iRsrv != 0)
                {
                    return reply->m_header.iRsrv;
                }
                else
                {
                    dataOut = std::move(reply->m_sData);
                    return Mcsf::McsfSuccess;
                }

            }
            else {
                NTB_LOG_INFO(command.sReceiver << " exit!");
                return Mcsf::McsfUndefineError;
            }

        }
        else {
            m_callHandlers.unregister_pending_call(p->m_header.uid);
            NTB_LOG_INFO("timeout for wait uid/id=" << p->m_header.uid << "/" << command.iCommandId);
            return Mcsf::McsfSendCommandTimeout;
        }
    }

    std::string CommunicationProxy::Implement::GetSystemDispatcherAddress(void) const
    {
        return m_pClient->get_dispatcher_addr();
    }

    int CommunicationProxy::Implement::AsyncSendCommand(const CommandContext* pCommandContext)
    {
        int erro = Mcsf::McsfSuccess;
        netbasecore::connection_ptr conn = build_connection(pCommandContext->sReceiver, erro);
        if (!conn)
        {
            NTB_LOG_INFO("build_connection with " << pCommandContext->sReceiver << " failed. erro=" << erro);
            return erro;
        }

        netbasecore::util::RpcMsgPackagePtr p(
            new netbasecore::util::RpcMsgPackage(netbasecore::util::msg_type_cmd));
        p->m_header.uid = get_uid();
        p->m_header.id = pCommandContext->iCommandId;
        p->setData(pCommandContext->sSerializeObject);

        netbasecore::pending_call_ptr call(
            new netbasecore::pending_call(pCommandContext->sReceiver, m_svrPool.get_reply_handle_service(), false,
                pCommandContext->pCommandCallback, pCommandContext->iWaitTime));
        call->fixTimer(p->m_header.uid, &m_callHandlers);
        m_callHandlers.register_pending_call(p->m_header.uid, call);

        NTB_LOG_INFO(GetName() << "=>" << pCommandContext->sReceiver << " c=" << pCommandContext->iCommandId << " u=" << p->m_header.uid << " l=" << p->m_header.uDataLen);

        if (!conn->push_message(p))
        {
            NTB_LOG_INFO("push message error!");
            return Mcsf::McsfSocketConnectFailed;
        }

        return erro;
    }

    int CommunicationProxy::Implement::SyncSendCommand(const CommandContext* pCommandContext, std::string& sSerializeObject)
    {
        int erro = Mcsf::McsfSuccess;
        netbasecore::connection_ptr conn = build_connection(pCommandContext->sReceiver, erro);
        if (!conn)
        {
            NTB_LOG_INFO("build_connection with " << pCommandContext->sReceiver << " failed. erro=" << erro);
            return erro;
        }
        return sync_send_command(conn, *pCommandContext, sSerializeObject);
    }

    Mcsf::ISyncResult* CommunicationProxy::Implement::SyncSendCommand(const CommandContext* pCommandContext)
    {
        Mcsf::SyncResult *pInnerResult = new SyncResult();
        
        int iRet = SyncSendCommand(pCommandContext, pInnerResult->m_sResult);
        pInnerResult->SetCallResult(iRet);

        return pInnerResult;
    }

    int CommunicationProxy::Implement::SendEvent(const std::string& sSender, int iChannelId,int iEventId, const std::string& sEvent)
    {
        return send_event(send_event_id, iChannelId, iEventId, sEvent);
    }

    int CommunicationProxy::Implement::BroadcastEvent(const std::string& sSender, int iChannelId, int iEventId, const std::string& sEvent)
    {
        return send_event(broad_cast_id, iChannelId, iEventId, sEvent);
    }

    int CommunicationProxy::Implement::SendSystemEvent(const std::string & sSender, int iChannelId, int iEventId, const std::string & sEvent)
    {
        return send_event(send_event_id, iChannelId, iEventId, sEvent);
    }

    int CommunicationProxy::Implement::BroadcastSystemEvent(const std::string & sSender, int iChannelId, int iEventId, const std::string & sEvent)
    {
        return send_event(broad_cast_id, iChannelId, iEventId, sEvent);
    }

    int CommunicationProxy::Implement::CheckCastName(const std::string & sName, int iWaitTime)
    {
        int errorCode = 0;
        netbasecore::connection_ptr conn = build_connection(sName, errorCode, iWaitTime);
        if (!conn)
        {
            NTB_LOG_INFO("build_connection with " << sName << " failed. erro=" << errorCode);
            return errorCode;
        }
        return Mcsf::McsfSuccess;
    }

    boost::shared_ptr<IDataSender> CommunicationProxy::Implement::create_sender(const std::string &sPeerIP) {
        boost::shared_ptr<IDataSender> pRtn;
        if (force_use_socket || sPeerIP != boost::asio::ip::address_v4::loopback().to_string()){
            pRtn.reset(new SocketSender);
            pRtn->SetName(sPeerIP);
        }
        else {
            pRtn.reset(new SMFileSender);
        }
        return pRtn;
    }

    int CommunicationProxy::Implement::send_event(int type, int iChannelId, int iEventId, const std::string &sEvent)
    {
        netbasecore::connection_ptr conn = m_connectionMngr.get_connection(dispatcher_name);
        if (conn == nullptr)
        {
            NTB_LOG_ERROR("send_event error for no connection with dispatcher!");
            return Mcsf::McsfUndefineError;
        }

        netbasecore::util::RpcMsgPackagePtr p(
            new netbasecore::util::RpcMsgPackage(netbasecore::util::msg_type_evt));
        p->m_header.uid = iEventId;
        p->m_header.id = type;
        p->m_header.iRsrv = iChannelId;
        p->setData(sEvent);

        NTB_LOG_INFO("event.e=" << iEventId << " l=" << p->m_header.uSenderLen);
        if (!conn->push_message(p))
        {
            NTB_LOG_ERROR("send_event error for can't push message!");
            return Mcsf::McsfUndefineError;
        }
        return Mcsf::McsfSuccess;
    }

    int CommunicationProxy::Implement::SyncSendDataEx(const SendDataContextEx& pSendContext, std::string& sSerializeObject)
    {
        if (pSendContext.pRawData == NULL || pSendContext.iLen <= 0)
        {
            return McsfUndefineError;
        }

        int erro = Mcsf::McsfSuccess;
        netbasecore::connection_ptr conn = build_connection(pSendContext.sReceiver, erro);
        if (!conn)
        {
            NTB_LOG_INFO("build_connection with " << pSendContext.sReceiver << " failed. erro=" << erro);
            return erro;
        }

        boost::shared_ptr<IDataSender> pSender = create_sender(conn->get_remote_addr());

        pSender->SetTimeOut(pSendContext.iWaitTime);
        if (false == pSender->SendRawData(pSendContext.pRawData, pSendContext.iLen))
        {
            return McsfSendDataFailed;
        }


        //NTB_LOG_INFO_WITH_THIS("Sender addr="<<sSendPeerIP);
        McsfSendDataRequest request;
        request.set_name(pSender->GetName());
        request.set_type(IDataSender::send_data_by_sharememory == pSender->GetSendType()
            ? McsfSendDataRequest_SendType_SHARE_MEMORY : McsfSendDataRequest_SendType_SOCKET);
        request.set_is_sync(true);
        request.set_content(pSendContext.sSerailizedObject);

        CommandContext context;
        context.sReceiver = pSendContext.sReceiver;
        context.iCommandId = blob_data_id;
        request.SerializeToString(&context.sSerializeObject);


        std::string dataOut;
        int iSend = sync_send_command(conn, context, dataOut);
        if (0 != iSend)
            return iSend;

        McsfSendDataResponse response;
        response.ParseFromString(dataOut);
        if (response.has_contents())
        {
            sSerializeObject = response.contents();
        }

        return response.is_success() ? McsfSuccess : McsfResponseSayFailed;
    }

    ISyncResult * CommunicationProxy::Implement::SyncSendDataEx(const SendDataContextEx & pSendContext)
    {
        SyncResult *pInnerResult = new SyncResult();
        int iRet = SyncSendDataEx(pSendContext, pInnerResult->m_sResult);
        pInnerResult->SetCallResult(iRet);

        return pInnerResult;
    }

    int CommunicationProxy::Implement::AsyncSendDataEx(const SendDataContextEx& pSendContext)
    {
        if (pSendContext.pRawData == NULL || pSendContext.iLen <= 0)
        {
            NTB_LOG_ERROR("invalid parameters!pRawData=" << pSendContext.pRawData.get() << " iLen=" << pSendContext.iLen);
            return McsfUndefineError;
        }

        int erro = Mcsf::McsfSuccess;
        netbasecore::connection_ptr conn = build_connection(pSendContext.sReceiver, erro);
        if (!conn)
        {
            NTB_LOG_INFO("build_connection with " << pSendContext.sReceiver << " failed. erro=" << erro);
            return erro;
        }

        boost::shared_ptr<IDataSender> pSender = create_sender(conn->get_remote_addr());
        if (false == pSender->SendRawData(pSendContext.pRawData, pSendContext.iLen))
        {
            return McsfSendDataFailed;
        }

        McsfSendDataRequest request;
        request.set_name(pSender->GetName());
        request.set_type(IDataSender::send_data_by_sharememory == pSender->GetSendType()
            ? McsfSendDataRequest_SendType_SHARE_MEMORY : McsfSendDataRequest_SendType_SOCKET);
        request.set_is_sync(false);
        request.set_content(pSendContext.sSerailizedObject);

        CommandContext context;
        context.iWaitTime = pSendContext.iWaitTime;
        context.sReceiver = pSendContext.sReceiver;
        context.iCommandId = blob_data_id;
        context.pCommandCallback = new AsyncBigDataCallBackHandler(pSender, pSendContext.pCallback);
        request.SerializeToString(&context.sSerializeObject);

        int isend = AsyncSendCommand(&context);
        if (0 != isend) {
            delete context.pCommandCallback;
        }
        return isend;
    }

    int CommunicationProxy::Implement::RegisterCommandHandler(int iCommandId, ICommandHandler* pCommandHandler)
    {
        m_callHandlers.register_cmd(iCommandId, pCommandHandler);
        NTB_LOG_INFO(GetName() << " RegisterCommandHandler id=" << iCommandId);
        return 0;
    }

    int CommunicationProxy::Implement::RegisterCommandHandlerEx(int iCommandId, boost::shared_ptr<ICommandHandlerEx> pCommandHandler)
    {
        m_callHandlers.register_cmd_ex(iCommandId, pCommandHandler);
        NTB_LOG_INFO(GetName() << " RegisterCommandHandlerEx id=" << iCommandId);
        return 0;
    }

    void CommunicationProxy::Implement::UnRegisterCommandHandler(int iCommandId)
    {
        m_callHandlers.unregister_cmd(iCommandId);

        NTB_LOG_INFO(GetName() << " UnRegisterCommandHandler id=" << iCommandId);
    }

    void CommunicationProxy::Implement::UnRegisterAllCommandHandlers(void)
    {
        m_callHandlers.clear_all_cmd();
        NTB_LOG_INFO(GetName() << " UnRegisterAllCommandHandlers ");
    }

    int CommunicationProxy::Implement::RegisterDataHandlerEx(boost::shared_ptr<IDataHandlerEx> pSendDataHandler)
    {
        if (pSendDataHandler == nullptr)
            return -1;

        boost::shared_ptr<ICommandHandlerEx> pDataCmdHandler(new SendDataCmdHandler(pSendDataHandler));
        m_callHandlers.register_cmd_ex(blob_data_id, pDataCmdHandler);
        NTB_LOG_INFO(GetName() << " register blob_data_id.");
        return 0;
    }

    void CommunicationProxy::Implement::UnRegisterDataHandler(void)
    {
        m_callHandlers.unregister_cmd(blob_data_id);
        NTB_LOG_INFO(GetName() << " UnRegisterDataHandler ");
    }

    int CommunicationProxy::Implement::subscribe_event(const std::set<int> &eventIDs) {
        if (eventIDs.empty())
            return 0;

        Mcsf::CommandContext sub;
        sub.iCommandId = subs_event_id;
        sub.iWaitTime = 1000;
        sub.sReceiver = dispatcher_name;

        for (auto it = eventIDs.begin(); it != eventIDs.end(); ++it) {
            sub.sSerializeObject += boost::lexical_cast<std::string>(*it);
            sub.sSerializeObject += ",";
        }
        boost::erase_last(sub.sSerializeObject, ",");
        netbasecore::connection_ptr conn = m_connectionMngr.get_connection(dispatcher_name);
        if (!conn)
            return McsfUndefineError;

        std::string sUnused;
        return sync_send_command(conn, sub, sUnused);
    }

    int CommunicationProxy::Implement::RegisterEventHandler(int iChannelId, int iEventId, IEventHandler* pEventHandler)
    {
        int iRtn = 0;
        m_callHandlers.register_event(iEventId, pEventHandler);
        if (GetName() != dispatcher_name) {
            if (0 != subscribe_event(boost::assign::list_of(iEventId))) {
                iRtn = McsfUndefineError;
            }

            {
                boost::mutex::scoped_lock lc(m_uidmx);
                m_setSubcribIDs.insert(iEventId);
            }
        }

        return iRtn;
    }

    void CommunicationProxy::Implement::UnRegisterEventHandler(int iChannelId, int iEventId)
    {
        m_callHandlers.unregister_evt(iEventId);
    }

    void CommunicationProxy::Implement::UnRegisterAllEventHandlers()
    {
        m_callHandlers.clear_all_evt();
    }

    void CommunicationProxy::Implement::UnRegisterAllHandlers()
    {
        m_callHandlers.clear_all_handlers();
    }
}
