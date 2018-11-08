#include "pending_call.h"
#include "callback_handle.hpp"

namespace netbasecore {

    pending_call::pending_call(const std::string & recver,
        boost::asio::io_service & srv,
        bool bSync,
        Mcsf::ICommandCallbackHandler* pCallBackHander,
        unsigned int iTimeout)
        : timer_(srv)
        , m_bSync(bSync)
        , receiver_(recver)
        , m_pCallBackhandler(pCallBackHander)
        , m_iTimeout(iTimeout) {
    }

    pending_call::~pending_call()
    {
        boost::system::error_code egnor;
        timer_.cancel(egnor);
    }

    void pending_call::fixTimer(size_t uid, callback_handle * callhandle)
    {
        if (!m_bSync && m_iTimeout && m_pCallBackhandler)
        {
            timer_.expires_from_now(boost::posix_time::millisec(m_iTimeout));
            timer_.async_wait(boost::bind(&callback_handle::handle_async_timeout,
                callhandle, boost::asio::placeholders::error, uid));
        }
    }

    bool pending_call::wait()
    {
        boost::mutex::scoped_lock l(mx_);
        if (reslut_)
            return true;

        if (m_iTimeout)
        {
            if (cond_.timed_wait(l, boost::posix_time::milliseconds(m_iTimeout))) {
                return true;
            }
            else {
                return false;
            }
        }
        else
        {
            cond_.wait(l);
            return true;
        }
    }

    void pending_call::notfiy(util::RpcMsgPackagePtr rep)
    {
        boost::mutex::scoped_lock l(mx_);
        reslut_ = rep;
        cond_.notify_one();
    }

    Mcsf::ICommandCallbackHandler* pending_call::getCallBackHandler()
    {
        return m_pCallBackhandler;
    }

}