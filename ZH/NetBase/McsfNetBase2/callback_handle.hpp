#ifndef CALLBACK_HANDLE_HPP
#define CALLBACK_HANDLE_HPP

#include <map>
#include <set>
#include <string>

#include <boost/thread.hpp>
#include <boost/foreach.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include "rpc.h"
#include "connection_i.hpp"
#include "icom_proxy.h"
#include "pending_call.h"
#include "mcsf_refined_log.h"
#include "id_def.h"

namespace netbasecore {

    class CmdContextExAdptor : public Mcsf::ICommandContextEx
    {
    public:
        virtual ~CmdContextExAdptor() {}

        CmdContextExAdptor(util::RpcMsgPackagePtr p, const std::string &nm, connection_ptr c) 
            : comming_(p), myname(nm), conn_(c)
        {}

        virtual std::string GetReceiver() const
        {
            return myname;
        }

        virtual std::string GetSender() const
        {
            return comming_->m_sSender;
        }

        virtual int GetCommandId() const
        {
            return comming_->m_header.id;
        }

        virtual std::string GetSerializeObject() const
        {
            return comming_->m_sData;
        }

        virtual int Reply(const std::string& replyObject) const
        {
            util::RpcMsgPackagePtr reply(new util::RpcMsgPackage(util::msg_type_cmd_reply));
            reply->m_header.id = comming_->m_header.id;
            reply->m_header.uid = comming_->m_header.uid;
            reply->setData(replyObject);

            conn_->push_message(reply);
            return 0;
        }

        virtual Mcsf::ICommandContextEx* Clone()
        {
            return new CmdContextExAdptor(*this);
        }

        virtual void Destory()
        {
            delete this;
        };

    private:
        util::RpcMsgPackagePtr comming_;
        const std::string &myname;
        connection_ptr conn_;
    };

    class AsyncResultAdaptor : public Mcsf::IAsyncResult
    {
    public:
        AsyncResultAdaptor(util::RpcMsgPackagePtr p) : comming_(p), call_reslut_(ReceiveResponse) {
 
        }

        virtual std::string GetMarshalObject() const {
            return comming_ ? comming_->m_sData : "";
        }
 
        void setCallResult(CallResult r) { call_reslut_ = r; }

        virtual bool IsCompleted() const { return ReceiveResponse == call_reslut_; }

        virtual CallResult GetCallResult() const {
            return call_reslut_;
        }

        virtual IAsyncResult* Clone() const {
            return new AsyncResultAdaptor(*this);
        }

        virtual void Destory() {
            delete this;
        }

    private:
        CallResult call_reslut_;
        util::RpcMsgPackagePtr comming_;
    };

    class callback_handle
    {
    public:
        explicit callback_handle(const std::string &name) {
            if (name.empty())
            {
                boost::uuids::random_generator rgen;
                boost::uuids::uuid uu = rgen();
                my_name_ = boost::uuids::to_string(uu);
            }
            else
            {
                my_name_ = name;
            }
        }

        std::string getMyName() const { return my_name_; }

        void register_cmd_ex(size_t id, McsfICommandHandlerExPtr pCommandHandlerEx) {
            boost::mutex::scoped_lock l(mx_);
            m_mapcmdEx[id] = pCommandHandlerEx;
        }

        void register_cmd(size_t id, Mcsf::ICommandHandler* pCmdHandler)
        {
            boost::mutex::scoped_lock l(mx_);
            m_mapcmdRaw[id] = pCmdHandler;
        }

        void unregister_cmd(size_t id) {
            boost::mutex::scoped_lock l(mx_);
            m_mapcmdRaw.erase(id);
            m_mapcmdEx.erase(id);
        }

        void unregister_evt(size_t id) {
            boost::mutex::scoped_lock l(mx_);
            m_mapEvt.erase(id);
        }

        void clear_all_cmd() {
            boost::mutex::scoped_lock l(mx_);
            m_mapcmdEx.clear();
            m_mapcmdRaw.clear();
        }

        void clear_all_evt() {
            boost::mutex::scoped_lock l(mx_);
            m_mapEvt.clear();
        }

        void clear_all_handlers()
        {
            boost::mutex::scoped_lock l(mx_);
            m_mapcmdEx.clear();
            m_mapcmdRaw.clear();
            m_mapEvt.clear();
        }

        void register_event(size_t id, Mcsf::IEventHandler *pEventhandler) {
            boost::mutex::scoped_lock l(mx_);
            m_mapEvt[id].insert(pEventhandler);
        }

        void register_pending_call(size_t uid, pending_call_ptr pPendingcall)
        {
            boost::mutex::scoped_lock l(mx_);
            m_mapPendingcall[uid] = pPendingcall;
        }

        void unregister_pending_call(size_t uid) {
            boost::mutex::scoped_lock l(mx_);
            m_mapPendingcall.erase(uid);
        }

        void handle_async_timeout(const boost::system::error_code &ec, size_t uid)
        {
            if (boost::asio::error::operation_aborted != ec)
            {
                pending_call_ptr pending = GetPendingcallErased(uid);
                if (pending)
                {
                    Mcsf::ICommandCallbackHandler *pCallback = pending->getCallBackHandler();
                    if (pCallback) {
                        util::RpcMsgPackagePtr p(new util::RpcMsgPackage(util::msg_type_cmd_reply));
                        p->setSender(my_name_);

                        boost::shared_ptr<AsyncResultAdaptor> pAsyncRet(new AsyncResultAdaptor(p));
                        pAsyncRet->setCallResult(Mcsf::IAsyncResult::TimeOut);
                        pCallback->HandleReply(pAsyncRet.get());
                    }
                }
            }
        }

        void handle_cmd_reply_no_block(util::RpcMsgPackagePtr p, connection_ptr c, boost::asio::io_service& io)
        {
            if (c->connection_stat() == connection::connection_disconnected) {
                //assert p==nullptr
                std::vector<pending_call_ptr> vecPendingcall = getPendingCallErased(c->peer_name());
                for (std::vector<pending_call_ptr>::iterator it = vecPendingcall.begin();
                    it != vecPendingcall.end(); ++it) {
                    (*it)->getSync() ? (*it)->notfiy(p) : io.post(boost::bind(&callback_handle::handle_async_reply, this, *it, p));
                }
            }
            else {
                if (echo_cmd_id != p->m_header.id)
                    NTB_LOG_INFO("c=" << p->m_header.id << " u=" << p->m_header.uid << " l=" << p->m_header.uDataLen);
                pending_call_ptr pending = GetPendingcallErased(p->m_header.uid);
                if (pending) {
                    pending->getSync() ? pending->notfiy(p) : io.post(boost::bind(&callback_handle::handle_async_reply, this, pending, p));
                }
            }
        }

        void handle_message(const util::RpcMsgPackagePtr p, connection_ptr c)
        {
            if (util::msg_type_cmd == p->m_header.iMsgType) {
                if (echo_cmd_id != p->m_header.id) 
                    NTB_LOG_INFO("begin c=" << p->m_header.id << " u=" << p->m_header.uid << " s=" << p->m_sSender);

                McsfICommandHandlerExPtr cmdHandler = FindCommandHandlerEx(p->m_header.id);
                if (cmdHandler) {
                    handle_message_cmd_ex(cmdHandler, p, c);
                }
                else {
                    p->m_header.iMsgType = util::msg_type_cmd_reply;
                    Mcsf::ICommandHandler *pCmdHandler = FindCommandHandler(p->m_header.id);
                    if (pCmdHandler != nullptr) {
                        hande_message_cmd(pCmdHandler, p);
                    }
                    else {
                        NTB_LOG_INFO(my_name_ << " can't find the command id=" << p->m_header.id);
                        p->setData("");
                        p->m_header.iRsrv = Mcsf::McsfNotRegisterCommandHandler;
                    }
                    c->push_message(p);
                }

                if (echo_cmd_id != p->m_header.id) 
                    NTB_LOG_INFO("end c=" << p->m_header.id << " u=" << p->m_header.uid);
            }
            else if (util::msg_type_evt == p->m_header.iMsgType) {
                handle_message_event(p);
            }
            else {
                NTB_LOG_ERROR("unknown message type=" << p->m_header.iMsgType);
            }
        }

    private:
        void handle_message_cmd_ex(McsfICommandHandlerExPtr cmdHandler, const util::RpcMsgPackagePtr p, connection_ptr c) {
            boost::shared_ptr<CmdContextExAdptor> pAdaptor(new CmdContextExAdptor(p, my_name_, c));
            try {
                cmdHandler->HandleCommand(pAdaptor);
            }
            catch (const std::exception &e) {
                NTB_LOG_ERROR(std::string("exception for:") + e.what());
                pAdaptor->Reply("");
            }
        }

        void hande_message_cmd(Mcsf::ICommandHandler *pCmdHandler, const util::RpcMsgPackagePtr p) {
            Mcsf::CommandContext rev;
            rev.iCommandId = p->m_header.id;
            rev.sReceiver = my_name_;
            rev.sSender = p->m_sSender;
            rev.sSerializeObject = std::move(p->m_sData); //copy1 unavoidable
            try {
                pCmdHandler->HandleCommand(&rev, &p->m_sData);
                p->m_header.uDataLen = (uint32_t)p->m_sData.size();
            }
            catch (...) {
                NTB_LOG_ERROR(my_name_ << " exception occurs. c=" << p->m_header.id);
                p->setData("");
                p->m_header.iRsrv = Mcsf::McsfHandlerHasException;
            }
        }

        void handle_message_event(const util::RpcMsgPackagePtr p) {
            std::set<Mcsf::IEventHandler*> setEventHandler = FindEventHandlers(p->m_header.id);
            if (!setEventHandler.empty()) {
                boost::posix_time::ptime t1 = boost::posix_time::second_clock::universal_time();
                if (getMyName() == dispatcher_name) {
                    BOOST_FOREACH(Mcsf::IEventHandler* evtHandler, setEventHandler)
                        evtHandler->HandleEvent(p->m_sSender, p->m_header.iRsrv, p->m_header.uid, p->m_sData);
                }
                else {
                    size_t realDataLen = p->m_header.uDataLen - p->m_header.uSenderLen;
                    std::string realSender = p->m_sData.substr(realDataLen);
                    p->m_sData.erase(realDataLen);
                    BOOST_FOREACH(Mcsf::IEventHandler* evtHandler, setEventHandler) {
                        try {
                            evtHandler->HandleEvent(realSender, p->m_header.iRsrv, p->m_header.id, p->m_sData);
                        }
                        catch (const std::exception &e) { NTB_LOG_ERROR(e.what()); }
                    }
                }

                boost::posix_time::ptime t2 = boost::posix_time::second_clock::universal_time();
                if ((t2 - t1).total_seconds() > 2)
                    NTB_LOG_INFO("long time event handler consumed. e=" << p->m_header.id << " s=" << p->m_sSender);
            }
        }

        void handle_async_reply(pending_call_ptr pending, util::RpcMsgPackagePtr p)
        {
            Mcsf::ICommandCallbackHandler *pCallback = pending->getCallBackHandler();
            if (pCallback) {
                boost::shared_ptr<AsyncResultAdaptor> pAsyncRet(new AsyncResultAdaptor(p));
                if (p == nullptr) {
                    pAsyncRet->setCallResult(Mcsf::IAsyncResult::TCPClose);
                }
                else if (p->m_header.iRsrv) {
                    pAsyncRet->setCallResult(Mcsf::IAsyncResult::NotHandler);
                }
                pCallback->HandleReply(pAsyncRet.get());
            }
        }

        std::vector<pending_call_ptr> getPendingCallErased(const std::string &sRecevier) {
            std::vector<pending_call_ptr> vecRtn;

            boost::mutex::scoped_lock l(mx_);
            for (std::map<size_t, pending_call_ptr>::iterator it = m_mapPendingcall.begin()
                ; it != m_mapPendingcall.end();)
            {
                if (it->second->getReceiver() == sRecevier)
                {
                    vecRtn.push_back(it->second);
                    it = m_mapPendingcall.erase(it);
                }
                else {
                    ++it;
                }
            }

            return vecRtn;
        }

        McsfICommandHandlerExPtr FindCommandHandlerEx(size_t id) {
            boost::mutex::scoped_lock l(mx_);
            std::map<size_t, McsfICommandHandlerExPtr >::iterator it = m_mapcmdEx.find(id);
            return it == m_mapcmdEx.end() ? McsfICommandHandlerExPtr() : it->second;
        }

        Mcsf::ICommandHandler * FindCommandHandler(size_t id) {
            boost::mutex::scoped_lock l(mx_);
            std::map<size_t, Mcsf::ICommandHandler* >::iterator it = m_mapcmdRaw.find(id);
            return it == m_mapcmdRaw.end() ? nullptr : it->second;
        }

        std::set<Mcsf::IEventHandler*> FindEventHandlers(size_t id) {
            boost::mutex::scoped_lock l(mx_);
            std::map<size_t, std::set<Mcsf::IEventHandler*> >::iterator it = m_mapEvt.find(id);
            return it == m_mapEvt.end() ? std::set<Mcsf::IEventHandler*>() : it->second;
        }

        pending_call_ptr GetPendingcallErased(size_t uid)
        {
            pending_call_ptr pRtn;
            boost::mutex::scoped_lock l(mx_);
            std::map<size_t, pending_call_ptr >::iterator it = m_mapPendingcall.find(uid);
            if (m_mapPendingcall.end() != it)
            {
                pRtn = it->second;
                m_mapPendingcall.erase(it);
            }
            return pRtn;
        }

    private:
        std::map< size_t, McsfICommandHandlerExPtr > m_mapcmdEx;
        std::map< size_t, Mcsf::ICommandHandler* > m_mapcmdRaw;
        std::map< size_t, std::set<Mcsf::IEventHandler*> > m_mapEvt;

        std::map<size_t, pending_call_ptr> m_mapPendingcall;
        std::string my_name_;
        boost::mutex mx_;
    };
}


#endif // !1
