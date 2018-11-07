#ifndef __pending_call__
#define __pending_call__

#include <boost/thread.hpp>

#include "rpc.h"
#include "icom_proxy.h"

namespace netbasecore {

    class callback_handle;
    class pending_call {
    public:
        pending_call(const std::string & recver, boost::asio::io_service &srv,
            bool bSync,
            Mcsf::ICommandCallbackHandler* pCallBackHander,
            unsigned int iTimeout);

        ~pending_call();

        void fixTimer(size_t uid, callback_handle * callhandle);

        bool wait();

        void notfiy(util::RpcMsgPackagePtr rep);

        Mcsf::ICommandCallbackHandler* getCallBackHandler();

        util::RpcMsgPackagePtr getReplyResult() { return reslut_; }

        bool getSync() { return m_bSync; }

        std::string getReceiver() { return receiver_; }

    private:
        bool m_bSync;
        Mcsf::ICommandCallbackHandler* m_pCallBackhandler;
        unsigned int m_iTimeout;
        boost::mutex mx_;
        boost::condition_variable_any cond_;
        util::RpcMsgPackagePtr reslut_;
        boost::asio::deadline_timer timer_;
        std::string receiver_;
    };

    typedef boost::shared_ptr<pending_call> pending_call_ptr;
}
#endif

