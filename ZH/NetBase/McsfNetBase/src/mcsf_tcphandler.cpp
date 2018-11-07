#include "stdafx.h"
#ifdef ACE_WIN32
#include <MSTCPiP.h>
#endif

namespace Mcsf {
    TcpHandler::TcpHandler( ACE_Reactor *reactor, Task_Base_Ptr pTask ,const std::string& sOpposite ) 
        : ACE_Event_Handler(reactor)
        ,m_sOpposite(sOpposite)
        ,m_pReactor(reactor)
        ,m_pTcpChannel(NULL)
        ,m_iTimeOutCount(0)
        ,m_bIsServer(false)
        , m_bSendfailedMask(false)
        ,m_bCleared(false)
    {
        CErrorHandler::RegisterToDump(this);
        NTB_TRACE_INFO_WITH_THIS("TcpHandler constructor enter.");
        MCSF_TRY
            m_pTcpChannel = TcpChannel::CreateTcpChannel( pTask , sOpposite);
            m_bIsServer = boost::dynamic_pointer_cast< TcpServer >(pTask) != NULL;
            NTB_TRACE_INFO_WITH_THIS("create "<<(m_bIsServer ? "server" : "client")<<" channel="<<m_pTcpChannel);
            CHECK_NULL_PTR(m_pTcpChannel);
        
        MCSF_CATCH(Mcsf::Exception)
            NTB_LOG_ERROR_WITH_THIS(e.what()<<" m_pTcpChannel="<<m_pTcpChannel<<" m_sOpposite="<<m_sOpposite);
        
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("TcpHandler: Constructor occurs exception"<<" m_pTcpChannel="<<m_pTcpChannel
                                    <<" m_sOpposite="<<m_sOpposite);
        MCSF_TRY_CATCH_END
    }

    TcpHandler::~TcpHandler()
    {
        CErrorHandler::UnRegisterToDump(this);
    }

    int TcpHandler::open()
    {
        NTB_TRACE_INFO_WITH_THIS("TcpHandler open enter.m_pTcpChannel="<<m_pTcpChannel
            <<" m_sOpposite="<<m_sOpposite);

        MCSF_TRY
            int bufsiz = ACE_DEFAULT_MAX_SOCKET_BUFSIZ*8;
            this->peer ().set_option (SOL_SOCKET, SO_SNDBUF,
                &bufsiz, sizeof bufsiz);
            this->peer ().set_option (SOL_SOCKET, SO_RCVBUF,
                &bufsiz, sizeof bufsiz);

            int v = 1;
            this->peer().set_option(IPPROTO_TCP, TCP_NODELAY, 
                &v, sizeof(v));
            
            int keepAlive = 1;
            this->peer().set_option(SOL_SOCKET, SO_KEEPALIVE, 
                (void*)&keepAlive,sizeof(keepAlive));

            if (-1 == m_pReactor->register_handler(this, ACE_Event_Handler::READ_MASK))
            {
                NTB_LOG_ERROR_WITH_THIS("could not register read handler .server?="<<m_bIsServer);
                return -1;
            }


            ACE_Time_Value const startup (10);
            ACE_Time_Value const interval(10);

            //schedule heart beat timer
            if (-1 == m_pReactor->schedule_timer (this, 0, startup, interval))
            {
                NTB_LOG_ERROR_WITH_THIS("could not schedule timer .server?="<<m_bIsServer);
                return -1;
            }
      

            return 0;

            MCSF_CATCH_ALL
                NTB_LOG_ERROR_WITH_THIS("TcpHandler: open occurs exception.m_pTcpChannel="<<m_pTcpChannel
                <<" m_sOpposite="<<m_sOpposite);
            MCSF_TRY_CATCH_END
            return McsfUndefineError;
    }

    int TcpHandler::handle_input( ACE_HANDLE fd)
    {
//        NTB_TRACE_INFO_WITH_THIS("TcpHandler handle_input enter.m_pTcpChannel="<<m_pTcpChannel);

        int iRtn = McsfSuccess;
        if (ACE_INVALID_HANDLE == fd || m_bSendfailedMask)
        {
            NTB_LOG_ERROR_WITH_THIS("ACE_INVALID_HANDLE == fd.m_bSendfailed=" << m_bSendfailedMask);
            return -1;
        }


        m_iTimeOutCount = 0;

        iRtn = m_pTcpChannel->HandleRead();

        return iRtn;
    }

    int TcpHandler::handle_close( ACE_HANDLE fd, ACE_Reactor_Mask mask )
    {
        NTB_TRACE_INFO_WITH_THIS("TcpHandler::handle_close enter.m_pTcpChannel="<<m_pTcpChannel<<" fd="<<fd<<" mask="<<mask
            <<" m_bIsServer="<<m_bIsServer);

        ClearResource();
        return 0;
    }

    int TcpHandler::handle_timeout(const ACE_Time_Value &current_time, const void *act )
    {
        ACE_UNUSED_ARG(current_time);
        ACE_UNUSED_ARG(act);

        if (!m_bIsServer) {

            if (m_bSendfailedMask)
            {
                NTB_LOG_ERROR_WITH_THIS("m_bSendfailed!!! handle_close will be called.server?=" << m_bIsServer);
                return -1;
            }

            if (0 != m_pTcpChannel->SendACK(message_type_ack_request)) //max 100 millisecond
            {
                ++m_iTimeOutCount;
                NTB_LOG_WARN_WITH_THIS("client SendACK failed!!! m_iTimeOutCount=" << m_iTimeOutCount);
            }
            else
            {
                m_iTimeOutCount = 0;
            }
        }
        else
        {
            ++m_iTimeOutCount;
        }

        if (m_iTimeOutCount < 3)
        {
            return 0;
        }
        else
        {
#if defined (_DEBUG) && defined(ACE_WIN32)
            // in debug state, the heart beat mechanism  is invalided
            return 0;
#else
            NTB_LOG_ERROR_WITH_THIS("timeout!!!" << m_iTimeOutCount << " handle_close will be called.server?=" << m_bIsServer);
            return -1; //call handle_close.
#endif
        }
    }

    void TcpHandler::ClearResource()
    {
        if (!m_bCleared)
        {
            m_bCleared = true;

            NTB_TRACE_INFO_WITH_THIS("TcpHandler::ClearResource enter.m_pTcpChannel=" << m_pTcpChannel);
            m_pReactor->cancel_timer(this);
            m_pReactor->remove_handler(this, ACE_Event_Handler::TIMER_MASK | ACE_Event_Handler::DONT_CALL);

            ACE_Reactor_Mask mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
            this->reactor()->remove_handler(this, mask);
            this->peer().close();
            m_pTcpChannel->HandleClose();
            m_pTcpChannel->Release();
            this->Release();
        }
    }

    std::string TcpHandler::Dump()
    {
        MCSF_TRY
            MCSF_DUMP(m_sDumpInfo, "(void)*", "this: ", this );
            MCSF_DUMP(m_sDumpInfo,"TcpChannel*", "m_pTcpChannel:", m_pTcpChannel);
            MCSF_DUMP(m_sDumpInfo, "std::string", "m_sOpposite:", m_sOpposite);
            return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
            return "";
        MCSF_TRY_CATCH_END
    }

}