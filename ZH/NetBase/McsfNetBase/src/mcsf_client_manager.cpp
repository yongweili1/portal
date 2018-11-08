#include "stdafx.h"
namespace Mcsf {

    const std::string sSystemDispatchName = "SystemDispatcher";
    ClientManager::ClientManager(ACE_Reactor* r) : m_iIdGenerator(0),
        m_clientMapMutex(), m_idMutex(), m_ufpc(0), m_pReactor(r)
    {
        CErrorHandler::RegisterToDump(this);
        m_pAsynCallbackThread.reset(new AsyncCallBackThread);
        m_pClient = boost::make_shared<TcpClient>(this);
    }

    ClientManager::~ClientManager()
    {
        CErrorHandler::UnRegisterToDump(this);
        RemoveAllServer();
        DestroyAllClient();
    }

    ACE_Reactor* ClientManager::GetReactor()
    {
        return m_pReactor;
    }

    CommandCallPtr ClientManager::SendRequest(const CommandContext* pContext,
        bool bNeedCallback, ICommandCallbackHandler* pCallback, IDataSender *pSendData)
    {
        int tmpuid = 0;
        NTB_TRACE_INFO_WITH_THIS("ClientManager SendRequest enter.");
        {
            ACE_GUARD_RETURN(ACE_Thread_Mutex, localMutex, m_idMutex, CommandCallPtr());
            tmpuid = ++m_iIdGenerator;
            //             NTB_TRACE_INFO_WITH_THIS("ClientManager SendRequest cmd_id="<<pContext->iCommandId<<" uid="<<m_iIdGenerator
            //                 <<" sender="<<pContext->sSender<<" receiver="<<pContext->sReceiver);
        }
        TcpClient_Ptr pTcpClient;
        {
            ACE_READ_GUARD_RETURN(ACE_RW_Thread_Mutex, readMutex, m_clientMapMutex, CommandCallPtr());
            Name2Client::const_iterator itr = m_mapClients.find(pContext->sReceiver);
            if (itr == m_mapClients.end())
            {
                NTB_LOG_ERROR_WITH_THIS("can't find the client of " << pContext->sReceiver << " m_iIdGenerator=" << m_iIdGenerator);
                return CommandCallPtr();
            }
            else
            {
                pTcpClient = itr->second;
            }
        }

        return pTcpClient->SendRequest(tmpuid, pContext, bNeedCallback, pCallback, pSendData);
    }

    void ClientManager::SendRequestWithoutRespond(RpcMsgPackage *pck, const std::string &sReceiver)
    {
        //pck not null forever.
        NTB_TRACE_INFO_WITH_THIS("ClientManager SendRequestWithoutRespond enter. Receiver=" << sReceiver);

        {
            ACE_READ_GUARD(ACE_RW_Thread_Mutex, readMutex, m_clientMapMutex);
            Name2Client::const_iterator itr = m_mapClients.find(sReceiver);
            if (itr != m_mapClients.end())
            {
                RpcMsgHeader *pHeader = pck->GetHeader();
                NTB_TRACE_INFO_WITH_THIS("ClientManager SendRequestWithoutRespond enter. Receiver=" << sReceiver
                    << " Sender=" << pck->GetSender()
                    << " iChannelId=" << pHeader->iRsrv << " iEventId=" << pHeader->id
                    << " uid=" << pHeader->uid);
                itr->second->SendRequestWithOutResponse(pck);
            }
            else
            {
                DEL_PTR(pck);
            }
        }
    }

    int ClientManager::CreateClient(const std::string& sContaineeName, const std::string& sIpAddr, bool bRetry)
    {
        NTB_TRACE_INFO_WITH_THIS("ClientManager CreateClient enter. sContaineeName=" << sContaineeName << " sIpAddr=" << sIpAddr);
        TcpClient_Ptr pClient;
        int iRet = 0;
        {
            boost::mutex::scoped_lock lc(m_clientMutex);
            pClient = m_pClient;

            NTB_LOG_INFO_WITH_THIS("[Connection] Connect to " << sContaineeName.c_str() << " (IPAddr:" << sIpAddr.c_str() << ")." << " m_iIdGenerator=" << m_iIdGenerator);
            iRet = pClient->Init(sIpAddr, sContaineeName, bRetry);
            m_pClient = boost::make_shared<TcpClient>(this);
            if (iRet != McsfSuccess)
            {
                NTB_LOG_ERROR_WITH_THIS("[Connection] Connect to " << sContaineeName.c_str() << " failed. iRet=" << iRet);
            }
            else
            {
                NTB_LOG_INFO_WITH_THIS("[Connection] Connect to " << sContaineeName.c_str() << " success.");
                RegisterClient(sContaineeName, pClient);
            }
        }

        return iRet;
    }

    int ClientManager::GetSize()
    {
        ACE_READ_GUARD_RETURN(ACE_RW_Thread_Mutex, readMutex, m_clientMapMutex, 0);
        return m_mapClients.size();
    }

    void ClientManager::RegisterClient(const std::string& sContaineeName,
        TcpClient_Ptr pClient)
    {
        NTB_TRACE_INFO_WITH_THIS("ClientManager RegisterClient enter. sContaineeName=" << sContaineeName);
        ACE_WRITE_GUARD(ACE_RW_Thread_Mutex, readMutex, m_clientMapMutex);
        m_mapClients[sContaineeName] = pClient;
    }

    void ClientManager::UnRegisterClient(const std::string& sContaineeName)
    {
        NTB_TRACE_INFO_WITH_THIS("ClientManager UnRegisterClient enter. sContaineeName=" << sContaineeName);
        ACE_WRITE_GUARD(ACE_RW_Thread_Mutex, readMutex, m_clientMapMutex);
        m_mapClients.erase(sContaineeName);
    }

    void ClientManager::UnRegisterClient(const TcpClient_Ptr pClient)
    {
        NTB_TRACE_INFO_WITH_THIS("ClientManager UnRegisterClient enter.");
        ACE_WRITE_GUARD(ACE_RW_Thread_Mutex, readMutex, m_clientMapMutex);
        Name2Client::const_iterator itr = m_mapClients.begin();
        for (; itr != m_mapClients.end(); itr++)
        {
            if (itr->second == pClient)
            {
                m_mapClients.erase(itr->first);
                break;
            }
        }
    }

    bool ClientManager::IsClientRegister(const std::string& sContaineeName)
    {
        NTB_TRACE_INFO_WITH_THIS("ClientManager IsClientRegister enter.sContaineeName=" << sContaineeName);
        ACE_READ_GUARD_RETURN(ACE_RW_Thread_Mutex, mx, m_clientMapMutex, false);
        return m_mapClients.find(sContaineeName) != m_mapClients.end();
    }

    void ClientManager::DestroyAllClient()
    {
        NTB_TRACE_INFO_WITH_THIS("ClientManager DestoryAllClient enter.");
        // handler close will invoke UnRegisterClient which maybe make iterator invalid
        std::vector<std::string> vtName;
        std::vector<TcpClient_Ptr> vtClients;
        {
            //ACE_WRITE_GUARD_RETURN(ACE_RW_Thread_Mutex, readMutex, m_clientMapMutex, -1);
            Name2Client::const_iterator itr = m_mapClients.begin();
            for (; itr != m_mapClients.end(); itr++)
            {
                vtClients.push_back(itr->second);
            }
        }

        BOOST_FOREACH(TcpClient_Ptr x, vtClients)
        {
            if (x != NULL && x->m_pTcpHandler != NULL)
            {
                x->m_pTcpHandler->ClearResource();
            }
        }
    }

    bool ClientManager::IsLocalHost(const std::string& sContaineeName)
    {
        ACE_WRITE_GUARD_RETURN(ACE_RW_Thread_Mutex, readMutex, m_clientMapMutex, false);

        Name2Client::const_iterator itr = m_mapClients.find(sContaineeName);

        if (itr != m_mapClients.end())
        {
            if (itr->second->GetNetAddr().is_loopback())
            {
                return true;
            }

            std::string sLocalAddr = TcpServer::GetIpInterface();
            std::vector<std::string> results;
            boost::algorithm::split(results, sLocalAddr, boost::is_any_of("&"));
            for (std::vector<std::string>::const_iterator itr_inner = results.begin();
                itr_inner != results.end(); itr_inner++)
            {
                if (*itr_inner == itr->second->GetNetAddr().get_host_addr())
                {
                    return true;
                }
            }
        }
        return false;
    }

    void ClientManager::SetCallBack(EVENTTYPE eEvtType, boost::function1<void, void*> ufpc)
    {
        NTB_TRACE_INFO_WITH_THIS("ClientManager SetCallBack enter.ufpc= " << ufpc);
        ACE_UNUSED_ARG(eEvtType);
        boost::mutex::scoped_lock lock(m_cbMutex);
        m_ufpc = ufpc;
    }

    void ClientManager::CancelCallBack(EVENTTYPE eEvtType)
    {
        NTB_TRACE_INFO_WITH_THIS("CancelCallBack enter.eEvtType= " << eEvtType);
        boost::mutex::scoped_lock lock(m_cbMutex);
        m_ufpc = 0;
    }

    void ClientManager::CancelAllCallback()
    {
        NTB_TRACE_INFO_WITH_THIS("CancelAllCallback enter.");
        std::vector<TcpClient_Ptr> vtClients;
        {
            ACE_WRITE_GUARD(ACE_RW_Thread_Mutex, readMutex, m_clientMapMutex);
            Name2Client::const_iterator itr = m_mapClients.begin();
            for (; itr != m_mapClients.end(); itr++)
            {
                vtClients.push_back(itr->second);
            }
        }

        BOOST_FOREACH(TcpClient_Ptr x, vtClients)
        {
            if (x != NULL)
            {
                x->CancelAllCallback();
            }
        }
    }

    void ClientManager::TrigReconnection()
    {
        NTB_LOG_INFO_WITH_THIS("ClientManager::TrigReconnection.");
        if (m_ufpc)
        {
            m_ufpc((void*)NULL);
        }
    }

    void ClientManager::AddSvrTcpHandler(TcpChannel* pChannel, TcpHandler* pHandler)
    {
        NTB_TRACE_INFO_WITH_THIS("ClientManager::AddSvrTcpHandler.pChannel=" << pChannel << " pHandler=" << pHandler);
        boost::mutex::scoped_lock lock(m_mapServerMutex);
        m_mapChannel2Handler[pChannel] = pHandler;
    }

    void ClientManager::RemoveAllServer()
    {
        NTB_TRACE_INFO_WITH_THIS("ClientManager::RemoveAllServer.");
        std::vector<TcpHandler*> vtHandlers;
        {
            Channel2Handler::iterator itr = m_mapChannel2Handler.begin();
            for (; itr != m_mapChannel2Handler.end(); itr++)
            {
                vtHandlers.push_back(itr->second);
            }
        }

        BOOST_FOREACH(TcpHandler* x, vtHandlers)
        {
            if (x != NULL)
            {
                x->ClearResource();
            }
        }
    }

    void ClientManager::RemoveServer(TcpChannel* pChannel)
    {
        NTB_TRACE_INFO_WITH_THIS("ClientManager::RemoveServer.pChannel=" << pChannel);
        boost::mutex::scoped_lock lock(m_mapServerMutex);
        m_mapChannel2Handler.erase(pChannel);
    }

    std::string ClientManager::Dump()
    {
        MCSF_TRY
            std::ostringstream om;
        char dot[] = ", ";
        om << "this: " << this << endl;
        BOOST_FOREACH(const Name2Client::value_type &val, m_mapClients)
        {
            om << val.first << dot << val.second << std::endl;
        }
        om << "m_iIdGenerator: " << m_iIdGenerator << endl;

        BOOST_FOREACH(const Channel2Handler::value_type &pt, m_mapChannel2Handler)
        {
            om << pt.first << dot << pt.second << std::endl;
        }
        om << "m_pReactor: " << m_pReactor << endl;
        m_sDumpInfo = om.str();
        return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
        return "";
        MCSF_TRY_CATCH_END

    }

    int ClientManager::GetOneUid()
    {
        ACE_GUARD_RETURN(ACE_Thread_Mutex, localMutex, m_idMutex, -1);
        return ++m_iIdGenerator;
    }

    std::string ClientManager::GetReceiverIp(const std::string &sReceiver)
    {
        ACE_READ_GUARD_RETURN(ACE_RW_Thread_Mutex, readMutex, m_clientMapMutex, "");
        Name2Client::iterator itFind = m_mapClients.find(sReceiver);
        if (itFind != m_mapClients.end())
        {
            return itFind->second->GetRemoteIP();
        }
        return "";
    }

}
