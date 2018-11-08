#include "stdafx.h"

#define time_shift (116444736000000000ULL); // (27111902 << 32) + 3577643008

namespace Mcsf {

    CommandCall::CommandCall( int iId, const std::string &sSender, const std::string &sRecv,
        bool bSync, boost::shared_ptr<AsyncCallBackThread> pActive )
        :m_iId (iId)
        ,m_pCallback (NULL)
        ,m_bCompleted(false)
        ,m_bTimeOutForTransBigData(false)
        ,m_bIsSyncCmd(bSync)
        ,m_pSendData(NULL)
        ,m_pClientCbActivObj(pActive)
        ,m_sender(sSender)
        ,m_recver(sRecv)
    {
    }

    CommandCall::~CommandCall()
    {
    }
    
    int CommandCall::Wait(unsigned int iWaitTime)
    {
        NTB_TRACE_INFO_WITH_THIS("command call wait enter.iWaitTime="<<iWaitTime<<"ms"<<" m_pCallback="<<m_pCallback);
        MCSF_TRY 
            int isec = iWaitTime/1000;
            int iusec = (iWaitTime%1000)*1000;
            ACE_Time_Value tvTimeOut(isec, iusec);
            return iWaitTime == 0 ? m_Event.wait( ) : m_Event.wait( &tvTimeOut, 0);        
        
        MCSF_CATCH(Mcsf::Exception)        
            NTB_LOG_ERROR_WITH_THIS(e.what()<<" m_iId="<<m_iId<<" m_pCallback="<<m_pCallback
            <<" m_bCompleted="<<m_bCompleted<<" m_bTimeOutForTransBigData="<<m_bTimeOutForTransBigData
            <<" m_bIsSyncCmd="<<m_bIsSyncCmd<<" m_pSendData="<<m_bIsSyncCmd);
            ACE_UNUSED_ARG(e);        
        MCSF_CATCH_ALL        
            NTB_LOG_ERROR_WITH_THIS("CommandCall::Wait occurs exception."<<" m_iId="<<m_iId<<" m_pCallback="<<m_pCallback
            <<" m_bCompleted="<<m_bCompleted<<" m_bTimeOutForTransBigData="<<m_bTimeOutForTransBigData
            <<" m_bIsSyncCmd="<<m_bIsSyncCmd<<" m_pSendData="<<m_bIsSyncCmd);        
        MCSF_TRY_CATCH_END

        return McsfUndefineError;
    }
    
    void CommandCall::ClearDataFile(bool bIsSyncCmd)
    {        
        MCSF_TRY
            if(!bIsSyncCmd && !m_bCompleted)
            {
                m_bCompleted = true;    
                // if m_pCallback is not null, i think it is asynchronous command

                if( m_pCallback != NULL )
                {
                    //can't run callback in reactor thread, so put into another thread
                    AsyncResult* pResult = new AsyncResult();
                    CHECK_NULL_PTR(pResult);
                    pResult->SetCallResult(IAsyncResult::TCPClose);
                    pResult->SetCompleted( );            
                    pResult->SetCallback( m_pCallback );            
                    ACE_Message_Block* mb = new ACE_Message_Block( sizeof(pResult) );
                    CHECK_NULL_PTR(mb);
                    memcpy( mb->wr_ptr(), &pResult, sizeof(pResult) );            
                    m_pClientCbActivObj->putq( mb );            
                }                    
            }            
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("CommandCall::ClearDataFile occurs exception."<<" m_iId="<<m_iId<<" m_pCallback="<<m_pCallback
            <<" m_bCompleted="<<m_bCompleted<<" m_bTimeOutForTransBigData="<<m_bTimeOutForTransBigData
            <<" m_bIsSyncCmd="<<m_bIsSyncCmd<<" m_pSendData="<<m_bIsSyncCmd);  
        MCSF_TRY_CATCH_END
        return;
    }

    void CommandCall::SetResult( RpcMsgPackPtr pReplyPck, bool bTimeout )
    {
        NTB_TRACE_INFO_WITH_THIS("CommandCall::SetResult enter.m_pCallback="<<m_pCallback<<" bTimeout="<<bTimeout);
        AsyncResult* pResult = NULL;
        ACE_Message_Block* mb = NULL;
        
        MCSF_TRY
        
            //If has completed, means it's before timeout response received, we can't 
            //invoke callback function twice         
            
            //boost::mutex::scoped_lock lock(m_mutexCallBack);
            if(m_bCompleted)
            {
                // delete this;
                return;
            }

            // if m_pCallback is not null, i think it is asynchronous command
            if( m_pCallback != NULL )
            {
                //can't run callback in reactor thread, so put into another thread
                pResult = new AsyncResult();
                CHECK_NULL_PTR(pResult);
                //one time memcpy 2
                if(bTimeout)
                {                         
                    if (!m_bTimeOutForTransBigData)
                    {
                        m_bCompleted = true;
                    }                    
                    pResult->SetCallResult(IAsyncResult::TimeOut);
                }
                else
                {
                    pResult->SetMarshalObject( pReplyPck ? pReplyPck->GetReceivedBuff() : ReceivedBuff() );
                    pResult->SetCallResult(IAsyncResult::ReceiveResponse);
                    m_bCompleted = true;
                }
                pResult->SetCompleted( );

                pResult->SetCallback( m_pCallback );

                mb = new ACE_Message_Block( sizeof(pResult) );
                CHECK_NULL_PTR(mb);
                memcpy( mb->wr_ptr(), &pResult, sizeof(pResult) );
                NTB_TRACE_INFO_WITH_THIS( "Get the response.");

                m_pClientCbActivObj->putq( mb );

                return;
            }

             if( pReplyPck != NULL )
             {
                 m_pReplyPck = pReplyPck;
             }
             else if(!bTimeout)
             {
                 m_pReplyPck.reset(new RpcMsgPackage);
                 RpcMsgHeaderPtr pHeader(new RpcMsgHeader);
                 pHeader->iRsrv = McsfUndefineError;
                 m_pReplyPck->SetReceivedFields(pHeader, ReceivedBuff());
                 NTB_LOG_INFO_WITH_THIS("peer exit.");
             }

            DEL_PTR(m_pSendData);
        
        MCSF_CATCH(Mcsf::Exception)
            DEL_PTR(pResult);
            DEL_PTR(mb);
            NTB_LOG_ERROR_WITH_THIS(e.what()<<" m_iId="<<m_iId<<" m_pCallback="<<m_pCallback
                <<" m_bCompleted="<<m_bCompleted<<" m_bTimeOutForTransBigData="<<m_bTimeOutForTransBigData
                <<" m_bIsSyncCmd="<<m_bIsSyncCmd<<" m_pSendData="<<m_bIsSyncCmd);
        
        MCSF_CATCH_ALL        
            DEL_PTR(pResult);
            DEL_PTR(mb);
            NTB_LOG_ERROR_WITH_THIS("CommandCall::SetResult occurs exception."<<" m_iId="<<m_iId<<" m_pCallback="<<m_pCallback
                <<" m_bCompleted="<<m_bCompleted<<" m_bTimeOutForTransBigData="<<m_bTimeOutForTransBigData
                <<" m_bIsSyncCmd="<<m_bIsSyncCmd<<" m_pSendData="<<m_bIsSyncCmd);

        MCSF_TRY_CATCH_END
        //synchronous command free memory outside
        m_Event.signal();
    }

    void CommandCall::SetCallBack( IDataSender *pSendData)
    {
        NTB_TRACE_INFO_WITH_THIS("Set callback  pCallback="<<pSendData );
        m_pSendData= pSendData;        
    }

    void CommandCall::SetCallBack( ICommandCallbackHandler* pCallback )
    {
        NTB_TRACE_INFO_WITH_THIS("Set callback  pCallback="<<pCallback );
        m_pCallback = pCallback;
    }

     void CommandCall::CancelCallback()
     {
         NTB_TRACE_INFO_WITH_THIS("CanceCallback enter pCallback= NULL" );
         ACE_GUARD( ACE_Thread_Mutex, localGuard, m_pClientCbActivObj->m_Mutex );
         m_pCallback = NULL;
     }
    //////////////////////////////////////////////////////////////////////////
    //

    AsyncCallBackThread::AsyncCallBackThread()
    {
        this->msg_queue()->high_water_mark(1024*1024*16);
        this->msg_queue()->low_water_mark(1024*1024*16);
        this->Start();
    } 

    AsyncCallBackThread::~AsyncCallBackThread()
    {
        this->Stop();
    }

//     AsyncCallBackThread* AsyncCallBackThread::GetInstance()
//     {
//         return ACE_Singleton<AsyncCallBackThread, ACE_Thread_Mutex>::instance();
//     }

    int AsyncCallBackThread::Start()
    {
        return this->activate();
    }

    int AsyncCallBackThread::Stop()
    {
        ACE_Message_Block *mb = new ACE_Message_Block();
        CHECK_NULL_PTR(mb);
        mb->msg_type( ACE_Message_Block::MB_STOP );
        this->putq( mb );
        return this->wait();
    }

    void AsyncCallBackThread::DoHandleReply( AsyncResult *pResult )
    {
        // pResult can't be empty.
// #ifdef ACE_WIN32
//         __try
//         {
//             pResult->GetCallback()->HandleReply( pResult );
//         }
//         __except(EXCEPTION_EXECUTE_HANDLER)
//         {
//             ACE_DEBUG((LM_DEBUG, "HandleReply => exception found by SEH occurs."));
//         }
// #else
        pResult->GetCallback()->HandleReply( pResult );
// #endif
    }

    int AsyncCallBackThread::svc( void )
    {
        NTB_TRACE_INFO_WITH_THIS("AsyncCallBackThread::svc() enter.");        
        AsyncResult* pResult = NULL;
        MCSF_TRY        
            while( true )
            {
                ACE_Message_Block *mb = 0;
                if( getq(mb) == -1 )
                {
                    NTB_LOG_ERROR_WITH_THIS("getq(mb) == -1");
                    break;
                }
                if( mb->msg_type() == ACE_Message_Block::MB_STOP )
                {                    
                    NTB_LOG_INFO("mb->msg_type() == ACE_Message_Block::MB_STOP");
                    mb->release();
                    break;
                }
                memcpy( &pResult, mb->rd_ptr(), sizeof(pResult));   

                
                if (pResult != NULL && pResult->GetCallback() != NULL)
                {
                    // double check                    
                    ACE_GUARD_RETURN( ACE_Thread_Mutex, localGuard, m_Mutex, 0 );
                    if (pResult != NULL && pResult->GetCallback() != NULL)
                    {
                        NTB_TRACE_INFO_WITH_THIS( "begin handle reply");
                        MCSF_TRY
                            DoHandleReply(pResult);
                        MCSF_CATCH(std::exception)
                            NTB_LOG_ERROR_WITH_THIS(e.what());
                        MCSF_TRY_CATCH_END

                        NTB_TRACE_INFO_WITH_THIS( "end handle reply");
                    }
                }
                mb->release();
                DEL_PTR(pResult);
            }        
            
        MCSF_CATCH_ALL        
            DEL_PTR(pResult);
            NTB_LOG_ERROR_WITH_THIS("handle reply exception.");
        
        MCSF_TRY_CATCH_END
        NTB_TRACE_INFO_WITH_THIS("AsyncCallBackThread::svc() exit.");
        return 0;
    }

    //////////////////////////////////////////////////////////////////////////
    //
    int AsyncCallTimeout::handle_timeout( const ACE_Time_Value &,
        const void *act )
    {
        NTB_TRACE_INFO_WITH_THIS("AsyncCallTimeout::handle_timeout enter. m_bShareMemData="<<m_bShareMemData);
        ACE_UNUSED_ARG(act);
        if (m_pCall)
        {
            if (m_bShareMemData)
            {   
                m_pCall->TransDataTimeOut(true);                
            }            
            m_pCall->SetResult(RpcMsgPackPtr(), true);
        }
        NTB_TRACE_INFO_WITH_THIS("AsyncCallTimeout::handle_timeout leave");
        // when return -1, handle_close will be called.
        return -1;  // don't repeat timer
    }

    int AsyncCallTimeout::handle_close( ACE_HANDLE , ACE_Reactor_Mask mask )
    {
        NTB_TRACE_INFO_WITH_THIS("AsyncCallTimeout::handle_close enter.m_bShareMemData="<<m_bShareMemData);
        mask = ACE_Event_Handler::ALL_EVENTS_MASK | 
            ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, mask);
        delete this;
        NTB_TRACE_INFO_WITH_THIS("AsyncCallTimeout::handle_close leave.");
        return 0;
    }

    boost::uint64_t GetCurrentUTCTime()
    {
        {
#ifdef MCSF_PLATFORM_POSIX
            timeval tv;
            gettimeofday(&tv, 0); //gettimeofday does not support TZ adjust on Linux.
            return (static_cast<boost::uint64_t>(tv.tv_sec)) * 1000 + tv.tv_usec / 1000;
#elif defined(MCSF_PLATFORM_WIN32)
            FILETIME ft;
            GetSystemTimeAsFileTime(&ft);	
            union {
                FILETIME as_file_time;
                boost::uint64_t as_integer; // 100-nanos since 1601-Jan-01
            } time_caster;
            time_caster.as_file_time = ft;
            time_caster.as_integer -= time_shift; // filetime is now 100-nanos since 1970-Jan-01
            return time_caster.as_integer / 10000UL; //in millisecond and cannot be before 1970-Jan-01
#endif
        }

    }

}