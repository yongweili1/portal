#include "stdafx.h"

namespace {
    class DataCmdTask : public ACE_Task<ACE_MT_SYNCH>
    {
    public:
        DataCmdTask();
        ~DataCmdTask() {
            this->Stop();
        }
        static DataCmdTask* GetInstance(void);

        int Stop();

        virtual int svc(void);
    };

    DataCmdTask::DataCmdTask()
    {
        this->activate();
    }

    DataCmdTask* DataCmdTask::GetInstance( void )
    {
        return ACE_Singleton<DataCmdTask, ACE_Thread_Mutex>::instance();
    }

    int DataCmdTask::svc( void )
    {
        NTB_TRACE_INFO_WITH_THIS("Big data command task thread start.");
        ACE_Message_Block *mb = 0;
        Mcsf::SendDataCmdHandler* pCmd = NULL;
        MCSF_TRY        
            while( true )
            {
                if( getq(mb) == -1 )
                {
                    NTB_LOG_ERROR_WITH_THIS("getq(mb) == -1.");
                    break;
                }

                if( mb->msg_type() == ACE_Message_Block::MB_STOP )
                {
                    mb->release();
                    break;
                }
                
                memcpy( &pCmd, mb->rd_ptr(), sizeof(pCmd));
                pCmd->ReceiveData();

                mb->release();
                mb = NULL;
                DEL_PTR(pCmd);
            }        
        MCSF_CATCH_ALL        
            NTB_LOG_ERROR_WITH_THIS("occrus exception.");
            DEL_PTR(pCmd);
            if (NULL != mb)
            {
                mb->release();
            }
        MCSF_TRY_CATCH_END

        return 0;
    }

    int DataCmdTask::Stop()
    {
        ACE_Message_Block *mb = new ACE_Message_Block();
        CHECK_NULL_PTR(mb);
        mb->msg_type( ACE_Message_Block::MB_STOP );
        this->putq( mb );
        return this->wait();
    }
}

namespace Mcsf 
{

    int CheckCastCmdHandler::HandleCommand( const CommandContext* , std::string* pReplyObject )
    {
        *pReplyObject = "";
//        NTB_TRACE_INFO_WITH_THIS( "receive an check cast cmd";
        return 0;
    }

    //////////////////////////////////////////////////////////////////////////
    //

    SendDataCmdHandler::SendDataCmdHandler( 
        IDataHandler* pHandler ) : m_pDataHandler( pHandler ), m_pDataHandlerEx()
    {

    }

    SendDataCmdHandler::SendDataCmdHandler( boost::shared_ptr<IDataHandlerEx> pHandler 
        ) : m_pDataHandlerEx(pHandler), m_pDataHandler(NULL)
    {

    }

    void SendDataCmdHandler::ReceiveData( void )
    {
        NTB_TRACE_INFO_WITH_THIS("Receive big data enter.");
        McsfSendDataRequest request;
        request.ParseFromString( m_pContext->GetSerializeObject() );
        NTB_TRACE_INFO_WITH_THIS("ReceiveData enter.file name="<<request.name() );

        boost::scoped_ptr<IDataReceiver> pReceiver;
        if( request.type() == McsfSendDataRequest_SendType_SHARE_MEMORY )
        {
            pReceiver.reset( new SMFileReceiver( request.name() ) ) ;
        }else
        {
            pReceiver.reset( new SocketReceiver(request.name()));
        }

        size_t iLen = 0;

        bool bIsSyncCmd = request.is_sync();
        boost::shared_array<char> pData = pReceiver->GetRawData( iLen , bIsSyncCmd);

        McsfSendDataResponse response;
        response.set_is_success(false);
        MCSF_TRY
            if( pData.get() != NULL )
            {
                if(m_pDataHandlerEx != NULL)
                {
                    boost::shared_ptr<SendDataContextEx> pDataContext(new SendDataContextEx);
                    CHECK_NULL_PTR(pDataContext.get());
                    pDataContext->sSender = m_pContext->GetSender();
                    pDataContext->sSerailizedObject = request.content();
                    pDataContext->pRawData = pData;
                    pDataContext->iLen = iLen;
                    m_pDataHandlerEx->HandleDataTrans(pDataContext);
                    response.set_contents(pDataContext->sResponseObject);
                    response.set_is_success( true );
                }
                else if(m_pDataHandler != NULL) 
                {
                    m_pDataHandler->HandleDataTrans( pData.get(), iLen );
                    response.set_is_success( true );
                }
                else
                {
                    NTB_LOG_ERROR_WITH_THIS("data handler is null, or not binding data handler!");
                    response.set_is_success( false );
                }
            }
            else
            {
                NTB_LOG_ERROR_WITH_THIS("GetRawData is null!");
                response.set_is_success(false);
            }

        MCSF_CATCH (Mcsf::Exception )
            response.set_is_success(false);
            NTB_LOG_ERROR_WITH_THIS(e.what()<<" m_pDataHandler="<<m_pDataHandler<<" m_pDataHandlerEx="<<m_pDataHandlerEx);        
        MCSF_CATCH_ALL        
            response.set_is_success(false);
            NTB_LOG_ERROR_WITH_THIS("unknown error in SendDataCmdHandler::ReceiveData!"
                <<" m_pDataHandler="<<m_pDataHandler<<" m_pDataHandlerEx="<<m_pDataHandlerEx);
        MCSF_TRY_CATCH_END

        m_pContext->Reply(response.SerializeAsString());
    }

    // separate the send data command handler to a separate thread in order not to block
    // command process. 
    void SendDataCmdHandler::HandleCommand( boost::shared_ptr<ICommandContextEx> pContext )
    {
        NTB_TRACE_INFO_WITH_THIS("process senddata command.");
        SendDataCmdHandler *pCmdHandler = this->Clone(); 
        pCmdHandler->m_pContext = pContext;

        ACE_Message_Block* mb = new ACE_Message_Block( sizeof(pCmdHandler) );
        CHECK_NULL_PTR(mb);
        memcpy( mb->wr_ptr(), &pCmdHandler, sizeof(pCmdHandler) );
        DataCmdTask::GetInstance()->putq( mb );
    }

    //////////////////////////////////////////////////////////////////////////
    //
    int EchoCmdHandler::HandleCommand( const CommandContext* pContext,
        std::string* pReplyObject )
    {
        *pReplyObject = pContext->sSerializeObject;
        return 0;
    } 
}
