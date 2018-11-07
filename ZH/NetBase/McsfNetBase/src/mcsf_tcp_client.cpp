#include "stdafx.h"


static const ACE_Time_Value kTime1(0, 200 * 1000); 
static const ACE_Time_Value kTime2(0, 300 * 1000); 

namespace Mcsf {
    class DefaultCmdCallBackHandler : public Mcsf::ICommandCallbackHandler
    {
    public:
        virtual int HandleReply(const Mcsf::IAsyncResult* )
        { 
            NTB_TRACE_INFO_WITH_THIS( "not set asynchronous command callback");
            return 0;
        }
    };

    DefaultCmdCallBackHandler g_DefaultCmdCallBackHandler;

    AsyncBigDataCallBackHandler::AsyncBigDataCallBackHandler( 
        boost::shared_ptr<Mcsf::IDataSender> pSender, 
        boost::shared_ptr<ICommandCallbackHandlerEx> pCallback) 
        : m_pFileSender(pSender), 
        m_pCallback(pCallback),m_bTimeOut(false)
    {       
    }

    int AsyncBigDataCallBackHandler::HandleReply(const Mcsf::IAsyncResult* pResult)
    {
        MCSF_TRY
        
            NTB_TRACE_INFO_WITH_THIS("AsyncBigDataCallBackHandler::HandleReply");
           
            if(((pResult->GetCallResult() == IAsyncResult::ReceiveResponse) && !m_bTimeOut) || (pResult->GetCallResult() == IAsyncResult::TimeOut))
            {
                Mcsf::AsyncResult* pInnerResult = new Mcsf::AsyncResult();
                CHECK_NULL_PTR(pInnerResult);
                if (pResult->GetCallResult() != IAsyncResult::TimeOut)
                {
                    McsfSendDataResponse response;
                    response.ParseFromString(pResult->GetMarshalObject());
                    ReceivedBuff py(new std::string(response.contents()));
                    pInnerResult->SetMarshalObject(py);
                }

                pInnerResult->SetCallResult(pResult->GetCallResult());
                pInnerResult->SetCompleted();
                boost::shared_ptr<Mcsf::IAsyncResult> result(pInnerResult);
                if(m_pCallback.get() != NULL )
                {
                    m_pCallback->HandleReply(result);
                }
            }

            if (pResult->GetCallResult() == IAsyncResult::TimeOut)
            {
                m_bTimeOut = true;
                return McsfSuccess;
            }
            else 
            {
                delete this;
                return McsfSuccess;
            }
        
        MCSF_CATCH(Mcsf::Exception)
            NTB_LOG_ERROR_WITH_THIS(e.what()<<" m_pFileSender="<<m_pFileSender<<" m_pCallback="<<m_pCallback
                                    <<" m_bTimeOut="<<m_bTimeOut);
        
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("occurs exception."<<" m_pFileSender="<<m_pFileSender
            <<" m_pCallback="<<m_pCallback<<" m_bTimeOut="<<m_bTimeOut);
        MCSF_TRY_CATCH_END

        delete this;
        return McsfUndefineError;
    }

    TcpClient::TcpClient( ClientManager* pManage
        ) : m_pManager( pManage ), m_sOpposite(""), m_pTcpHandler(NULL), m_flag(false)
    {
        CErrorHandler::RegisterToDump(this);
        NTB_TRACE_INFO_WITH_THIS("TcpClient constructor enter.m_sRemoteAddr="<<m_sRemoteAddr);
        this->msg_queue()->high_water_mark(1024*1024*16);
        this->msg_queue()->low_water_mark(1024*1024*16);
        StartThread();
    }

    TcpClient::~TcpClient()
    {
        CErrorHandler::UnRegisterToDump(this);
        StopThread();
    }

    int TcpClient::Init( const std::string& sRemoteAddr, const std::string& sOpposite, bool bRetry )
    {
        NTB_TRACE_INFO_WITH_THIS("Init enter.sOpposite="<<sOpposite);
        MCSF_TRY
            m_sRemoteAddr = sRemoteAddr;
            std::vector<std::string> result;
            std::string port;
            if (m_sRemoteAddr.empty())
            {
                NTB_LOG_ERROR_WITH_THIS("m_sRemoteAddr is empty.");
                return McsfUndefineError;
            }

            boost::algorithm::split(result, m_sRemoteAddr, boost::is_any_of("&:"));
            port = result.back();
            result.pop_back();
            NTB_TRACE_INFO_WITH_THIS("m_sRemoteAddr="<<m_sRemoteAddr);

            int iRet = McsfSuccess;
            m_sOpposite = sOpposite;
            //m_pTcpHandler = new TcpHandler( ACE_Reactor::instance(),  this->shared_from_this());
            m_pTcpHandler = new TcpHandler( m_pManager->GetReactor(),  this->shared_from_this(), sOpposite);
            CHECK_NULL_PTR(m_pTcpHandler);
            NTB_TRACE_INFO_WITH_THIS("TcpClient::Init duplicate tcphandler="<<m_pTcpHandler);
            m_pTcpHandler->Duplicate(2);
            m_pTcpHandler->GetChannel()->Duplicate();
            NTB_TRACE_INFO_WITH_THIS("Init enter.m_pTcpHandler="<<m_pTcpHandler);

            std::vector<ACE_Time_Value> vtTimeout
                = boost::assign::list_of(kTime1)(kTime2);
            //first we use short time out to connect ,then we use long time out to connect 
            for (std::vector<std::string>::const_iterator itr = result.begin(); itr != result.end(); itr++)
            {
                bool blocalIP = *itr == "127.0.0.1";
                std::string ipPort = *itr + ":" + port;
                m_NetAddr.set(ipPort.c_str());

                int i = 0;
                for (std::vector<ACE_Time_Value>::const_iterator itr_timeout = vtTimeout.begin();
                    itr_timeout != vtTimeout.end(); itr_timeout++, i++)
                {
                    //第一次(short time)且对方非本地IP的话，才bind不同client IP进行重试
                    iRet = m_Connector.Connect(m_pTcpHandler, m_NetAddr, *itr_timeout, !(i || blocalIP));
                    //如果是本地IP或者设置了不重试，则break.
                    if (iRet == McsfSuccess || blocalIP || !bRetry)
                    {
                        break;
                    }
                }

                if (iRet == McsfSuccess)
                {
                    break;
                }
            }

            if(iRet != McsfSuccess)
            {
                NTB_LOG_ERROR_WITH_THIS( "Connect to " << m_sRemoteAddr << " failed. iRet="<<iRet );
                //we need to consider connect failed condition to avoid handler leak
                //we need to delete TcpHandler -> TcpChannel and smart point of myself. 
                if (NULL != m_pTcpHandler)
                {
                    //std::cout<<"################################"<<std::endl;
                    m_pTcpHandler->GetChannel()->Release(2);
                    m_pTcpHandler->Release(3);
                    m_pTcpHandler = NULL;
                    //m_pTcpHandler->handle_close();
                    //StartThread();
                }
                return iRet;
            }

            iRet = SendVeriInfo();
            if (-1 == iRet)
            {
                NTB_LOG_ERROR_WITH_THIS("SendVeriInfo failed.iRet="<<iRet<<" m_sOpposite="<<m_sOpposite<<" m_sRemoteAddr="<<m_sRemoteAddr);
                return iRet;
            }

            m_pTcpHandler->GetChannel()->Release();
            m_pTcpHandler->Release();

            return iRet;
        
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Exception occurs.m_sOpposite="<<m_sOpposite<<" m_sRemoteAddr="<<m_sRemoteAddr);
            return  McsfUndefineError;
        MCSF_TRY_CATCH_END
    }

    int TcpClient::StartThread()
    { 
        int iRet = 0;
        iRet = this->activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED );
        NTB_TRACE_INFO_WITH_THIS("start send thread.");
        if (-1 == iRet)
        {
            NTB_LOG_ERROR_WITH_THIS("iRet="<<iRet<<" m_sOpposite="<<m_sOpposite<<" m_sRemoteAddr="<<m_sRemoteAddr);
        }
        return iRet;
    }

    void TcpClient::StopThread()
    {
        if (!m_flag)
        {
            //std::cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<std::endl;
            ACE_Message_Block *mb=NULL;
            mb = new ACE_Message_Block();
            CHECK_NULL_PTR(mb);
            mb->msg_type( ACE_Message_Block::MB_STOP );
            this->putq( mb );
            if (0 != this->wait())
            {
                NTB_LOG_ERROR_WITH_THIS("wait thread exit failed.");
            }
        }
    }

    int TcpClient::SendVeriInfo()
    {
        NTB_TRACE_INFO_WITH_THIS("SendVeriInfo enter.");        

        const static int iSize = 64;
        ACE_Message_Block* pMb = new ACE_Message_Block(iSize);
        CHECK_NULL_PTR(pMb);
        char buf[iSize+1] = {0};
        //create random number
        srand((unsigned)time(NULL));
        for (int i=0; i<iSize-1; i++)
        {
            buf[i] = (rand()%256) + '0';
        }

        //crc verification
        boost::crc_optimal<8, 0x1021> crc_8bit;
        crc_8bit.process_bytes(buf, iSize-1);
        boost::uint8_t cs=crc_8bit.checksum();
        buf[iSize-1] = cs+'0';
        memcpy(pMb->wr_ptr(), buf, iSize);
        pMb->wr_ptr( iSize );
        NTB_TRACE_INFO_WITH_THIS("begin to send verification info.iSize="<<iSize);        
        int iRtn = m_pTcpHandler->GetChannel()->Send(pMb);
        NTB_TRACE_INFO_WITH_THIS("end to send verification info.iSize="<<iSize<<" rtn="<<iRtn);        
        pMb->release();
        return iRtn;
    }

    int TcpClient::svc( void )
    {
        NTB_LOG_INFO_WITH_THIS("TcpClient::svc enter.");
        ACE_Message_Block *mb = NULL;
        MCSF_TRY
            while( true )
            {
                if( -1 == this->getq(mb) )
                {
                    NTB_LOG_ERROR_WITH_THIS("Get queue data failed.m_sOpposite="<<m_sOpposite<<" m_sRemoteAddr="<<m_sRemoteAddr);
                    break;
                }

                CHECK_NULL_PTR(mb);

                // shutdown TCP client, and exit thread 
                if( mb->msg_type() == ACE_Message_Block::MB_STOP )
                {
                    NTB_TRACE_INFO_WITH_THIS("receive MB_STOP message.m_sOpposite="<<m_sOpposite<<" m_sRemoteAddr="<<m_sRemoteAddr);
                    mb->release();
                    break;
                }

                RpcMsgPackage *pReqRaw = NULL;
                memcpy( &pReqRaw, mb->rd_ptr(), sizeof( pReqRaw ) );
                mb->release();

                RpcMsgPackPtr pRequest(pReqRaw);

                TcpChannel *pChannel = m_pTcpHandler->GetChannel();
                if(NULL == pChannel || 0 != pChannel->Send( pRequest ) )
                {
                    m_pTcpHandler->SetSendFailedMask(true);
                    NTB_LOG_ERROR_WITH_THIS("Client send failed.m_sOpposite="<<m_sOpposite<<" m_sRemoteAddr="<<m_sRemoteAddr);
                    break;
                }
            }

            m_flag = true;
            if (NULL != m_pTcpHandler)
            {
                m_pTcpHandler->Release();
            }

        
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("occurs exception.m_sOpposite="<<m_sOpposite<<" m_sRemoteAddr="<<m_sRemoteAddr);
            if (NULL != mb)
            {
                mb->release();
                mb = NULL;
            }
            if (NULL != m_pTcpHandler)
            {
                m_pTcpHandler->Release();
            }
        MCSF_TRY_CATCH_END
            NTB_LOG_INFO_WITH_THIS("TcpClient::svc exit.m_sOpposite="<<m_sOpposite<<" m_sRemoteAddr="<<m_sRemoteAddr);
            return 0;
    }

    CommandCallPtr TcpClient::SendRequest(int uid, const CommandContext *pContext,
                                            bool bNeedCallback, ICommandCallbackHandler* pCallback,IDataSender *pSendData)
    {
        if (SYSTEM_COMMAND_ID_SYS_MANAGER_GET_PROCESS != pContext->iCommandId)
        {
            NTB_LOG_INFO(bNeedCallback << " " << pContext->sSender
                << "=>" << pContext->sReceiver << " c=" << pContext->iCommandId << " u=" << uid);
        }

        ACE_Message_Block *mb = NULL;
        MCSF_TRY
            RpcMsgPackage *psendPck(new RpcMsgPackage);
            psendPck->SetSentFields(uid,
                pContext->iCommandId,
                pContext->sSerializeObject,
                !bNeedCallback,
                message_type_cmd,
                pContext->sSender,
                false);

            int iBytesize = sizeof(psendPck);
            mb = new ACE_Message_Block(iBytesize);
            memcpy(mb->wr_ptr(), &psendPck, iBytesize);
            mb->wr_ptr(iBytesize);
            CommandCallPtr pCall = boost::make_shared<CommandCall>(uid, pContext->sSender, 
                pContext->sReceiver, !bNeedCallback, m_pManager->GetAysnClientActiveObj());

            // make the lock range as small as possible
            {
                ACE_GUARD_RETURN( ACE_Thread_Mutex, localGuard, m_mapMutex, CommandCallPtr() );
                NTB_TRACE_INFO_WITH_THIS("m_mapPendingCalls.size="<<m_mapPendingCalls.size());
                m_mapPendingCalls[uid] = pCall;
            }
            
            if (bNeedCallback)
            {
                ICommandCallbackHandler *pCbHandler = pContext->pCommandCallback;

                pCallback ? pCall->SetCallBack(pCallback) : pCall->SetCallBack(pCbHandler ? pCbHandler : &g_DefaultCmdCallBackHandler);
            }
            else
            {
                NTB_TRACE_INFO_WITH_THIS("Send sync cmd with cmd_id = "<<pContext->iCommandId);
                pCall->SetCallBack(pSendData);
                if (NULL != pSendData)
                {
                    NTB_TRACE_INFO_WITH_THIS("Send sync data by share memory!");
                    pSendData->CloseDataFile();
                }
            }

            if( this->putq( mb ) == -1 )
            {
                NTB_LOG_ERROR_WITH_THIS("Insert message into the message queue failed.m_sOpposite="<<m_sOpposite<<" m_sRemoteAddr="<<m_sRemoteAddr);
                return CommandCallPtr();
            }
            
            if ( bNeedCallback && pContext->iWaitTime != 0)
            {
                NTB_TRACE_INFO_WITH_THIS("pContext->iWaitTime="<<pContext->iWaitTime);
                int isec = pContext->iWaitTime/1000;
                int iusec = (pContext->iWaitTime%1000)*1000;
                ACE_Time_Value delay(isec, iusec);
                AsyncCallTimeout *pAsyncCallTimeout = new AsyncCallTimeout();
                CHECK_NULL_PTR(pAsyncCallTimeout);
                pAsyncCallTimeout->SetCommandCall(pCall);
                if (pContext->iCommandId == SYSTEM_COMMAND_ID_SHAREMEMORY_BIGDATA_TRANSFER)
                {
                    pAsyncCallTimeout->SetIsShareMemData(true);
                }
                //ACE_Reactor::instance()->schedule_timer( pAsyncCallTimeout, NULL, delay );
                m_pManager->GetReactor()->schedule_timer( pAsyncCallTimeout, NULL, delay );
                {
                    ACE_GUARD_RETURN( ACE_Thread_Mutex, localGuard, m_mapMutex, CommandCallPtr() );
                    m_mapTimeoutCalls[uid] = pAsyncCallTimeout;
                }
            }
            return pCall;
        
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Exception happen.m_sOpposite="<<m_sOpposite<<" m_sRemoteAddr="<<m_sRemoteAddr);
            if (NULL != mb)
            {
                mb->release();
            }
            return CommandCallPtr();
        MCSF_TRY_CATCH_END
    }

    int TcpClient::SendRequestWithOutResponse( RpcMsgPackage *pck )
    {
        //pck not null forever.
        ACE_Message_Block *mb = NULL;
        MCSF_TRY
            int iBytesz = sizeof(pck);
            mb = new ACE_Message_Block(iBytesz);
            CHECK_NULL_PTR(mb);
            memcpy(mb->wr_ptr(), &pck, iBytesz);
            mb->wr_ptr(iBytesz);
            return this->putq(mb);
        
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Exception happen.m_sOpposite="<<m_sOpposite<<" m_sRemoteAddr="<<m_sRemoteAddr);
            if (NULL != mb)
            {
                mb->release();
            }

            DEL_PTR(pck);
            return McsfUndefineError;
        MCSF_TRY_CATCH_END
    }

    void TcpClient::ClearDataFile()
    {
        CommandCallPtr pCall;
        
        MCSF_TRY

        ACE_GUARD( ACE_Thread_Mutex, localGuard, m_mapMutex );
        CommandCallMap::iterator itr = m_mapPendingCalls.begin();        
        bool bIsSyncCmd = false;
        for (; itr!= m_mapPendingCalls.end();)
        {                    
            pCall = itr->second; 
            bIsSyncCmd = pCall->IsSyncCmd();
            if (bIsSyncCmd)
            {
                ACE_Time_Value tv(0, 2000);//2ms
                ACE_OS::sleep(tv);
                pCall->SetResult(RpcMsgPackPtr(), false);
            }
            pCall->ClearDataFile(bIsSyncCmd);
            m_mapPendingCalls.erase(itr++);
        }
        
        AsyncCallTimeoutMap::iterator itrTimeout = m_mapTimeoutCalls.begin();
        for (; itrTimeout != m_mapTimeoutCalls.end();)
        {
            m_mapTimeoutCalls.erase(itrTimeout++);
        }

        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("TcpClient::ClearDataFile: Exception happen.m_sOpposite="<<m_sOpposite<<" m_sRemoteAddr="<<m_sRemoteAddr);        
        MCSF_TRY_CATCH_END
    }


    void TcpClient::ReceiveResponse( RpcMsgPackPtr resultMsg )
    {
        //resultMsg never not null
        RpcMsgHeader *pHeader = resultMsg->GetHeader();
        const int &uid = pHeader->uid;
        const int &cmdId = pHeader->id;

        if (SYSTEM_COMMAND_ID_SYS_MANAGER_GET_PROCESS != cmdId)
        {
            NTB_LOG_INFO("c=" << cmdId << " u=" << uid);
        }

        CommandCallPtr pCall;
        AsyncCallTimeout *pAsyncTimeout = NULL;
        MCSF_TRY
        {
            ACE_GUARD( ACE_Thread_Mutex, localGuard, m_mapMutex );
            CommandCallMap::iterator itr = m_mapPendingCalls.find( uid );
            if( itr != m_mapPendingCalls.end() )
            {
                pCall = itr->second;
                m_mapPendingCalls.erase(itr);
            }

            AsyncCallTimeoutMap::iterator itrTimeout = m_mapTimeoutCalls.find( uid );
            if( itrTimeout != m_mapTimeoutCalls.end() )
            {
                pAsyncTimeout = itrTimeout->second;
                m_mapTimeoutCalls.erase( itrTimeout );
            }
        }

        if (pCall)
        {
            // cancel timer
            if (NULL != pAsyncTimeout )
            {
                if (!pCall->IsCompleted())
                {
                    if (!pCall->IsTransDataTimeOut())
                    {
                        m_pManager->GetReactor()->cancel_timer(pAsyncTimeout, 0);
                    }
                }
            }

            pCall->SetResult( resultMsg );

        }

        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("TcpClient::ReceiveResponse: Exception happen.m_sOpposite="<<m_sOpposite<<" m_sRemoteAddr="<<m_sRemoteAddr);        
        MCSF_TRY_CATCH_END
    }

    int TcpClient::close( u_long flags /*= 0*/ )
    {
        // close is hook function when thread exit, here i can do crazy thing
        // such as delete this.
        ACE_UNUSED_ARG( flags );
        NTB_TRACE_INFO_WITH_THIS( "client thread will exit." );
        return 0;
    }   

    void TcpClient::CancelAllCallback()
    {
        NTB_TRACE_INFO_WITH_THIS("cancel all callback.");
        ACE_GUARD( ACE_Thread_Mutex, localGuard, m_mapMutex );
        CommandCallMap::iterator itr = m_mapPendingCalls.begin() ;
        for( ;itr != m_mapPendingCalls.end(); itr++)
        {
            itr->second->CancelCallback();
        } 
    }   

    void TcpClient::RemovePendingCall(CommandCallPtr pCmdCall)
    {
        CommandCallMap::iterator itr = m_mapPendingCalls.begin();
        for (; itr!= m_mapPendingCalls.end(); itr++)
        {
            if (itr->second == pCmdCall)
            {
                m_mapPendingCalls.erase(itr);
                break;
            }
        }
    }

    void TcpClient::RemoveTimeoutCall(AsyncCallTimeout* pCallTimeout)
    {
        AsyncCallTimeoutMap::iterator itr = m_mapTimeoutCalls.begin();
        for (; itr != m_mapTimeoutCalls.end(); itr++)
        {
            if (itr->second == pCallTimeout)
            {
                m_mapTimeoutCalls.erase(itr);
                break;
            }
        }
        
    }

    std::string TcpClient::Dump()
    {
        MCSF_TRY
            std::ostringstream om;
            char dot[] = ", ";
            om << "this:" << this << endl;
            om << " m_sOpposite:" << m_sOpposite << endl;
            om << " m_sRemoteAddr:" << m_sRemoteAddr << endl;
            om << " ip address:" << m_NetAddr.get_ip_address() <<endl;
            om << " m_pTcpHandler:" << m_pTcpHandler << endl;
            om << " m_pManager:" << m_pManager << endl;
            om << " m_pClientRecord:" << m_pClientRecord << endl;
            om << " flag:" << m_flag <<endl;

            BOOST_FOREACH(const AsyncCallTimeoutMap::value_type &pt, m_mapTimeoutCalls)
            {
                om << pt.first << dot << pt.second << std::endl;
            }
            m_sDumpInfo = om.str();
            return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
            return "";
        MCSF_TRY_CATCH_END
    }
}  
