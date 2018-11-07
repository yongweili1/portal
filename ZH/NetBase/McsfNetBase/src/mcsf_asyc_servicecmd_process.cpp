#include "stdafx.h"


namespace Mcsf {


    AsycServiceCmdProcess::AsycServiceCmdProcess( 
        RpcMsgPackPtr pRequest,
        TcpChannel* pChannel, IServerProcessCB *pCb,TcpServer* pTcpSvr
        ) : m_pRequest( pRequest ), m_pChannal( pChannel ),
        m_pCb(pCb),m_pTcpSvr(pTcpSvr)
    {
        ACE_ASSERT( m_pChannal != NULL && m_pCb != NULL );
        CErrorHandler::RegisterToDump(this);
    }

    AsycServiceCmdProcess::~AsycServiceCmdProcess()
    {
        CErrorHandler::UnRegisterToDump(this);
    }

    int AsycServiceCmdProcess::svc()
    {
        NTB_TRACE_INFO_WITH_THIS("Asyc Service Command Process enter.");
        int iRtn = McsfSuccess;
        MCSF_TRY

            RpcMsgHeader *pHeader = m_pRequest->GetHeader();
            const uint32_t &cmdevtid = pHeader->id;
            const uint32_t &uid = pHeader->uid;
            const std::string &sender = m_pRequest->GetSender();

            NTB_LOG_INFO_WITH_THIS("begin to handle command. id="<<uid<<" cmd_id="<<cmdevtid <<" Sender=" << sender );

            std::string sResultMsg;
            iRtn = m_pCb->HandleRpcCommand( m_pRequest, &sResultMsg, m_pChannal );

            NTB_LOG_INFO_WITH_THIS("end to handle command. id="<<uid<<" cmd_id="<<cmdevtid<<" Sender=" << sender);

            if (iRtn != IServerProcessCB::Not_Send_Response)
            {
                RpcMsgPackPtr pResult(new RpcMsgPackage);
                uint32_t iError = 0;
                if ( McsfSuccess != iRtn)
                {
                    sResultMsg.clear();
                    iError = IServerProcessCB::Not_Handler == iRtn
                        ? McsfNotRegisterCommandHandler : McsfHandlerHasException;
                }

                pResult->SetSentFields(uid, cmdevtid, sResultMsg, true, 0, "", false, iError);
                m_pChannal->Send( pResult );

                //remember to decrease the reference count
                m_pChannal->Release();
            }
        MCSF_CATCH(Mcsf::Exception)
            NTB_LOG_ERROR_WITH_THIS(e.what()<<" m_pChannal="<<m_pChannal);
            iRtn = McsfUndefineError;
        
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("AsycServiceCmdProcess::svc occurs exception."<<" m_pChannal="<<m_pChannal);
            iRtn = McsfUndefineError;
        MCSF_TRY_CATCH_END
        return iRtn;
    }

    int AsycServiceCmdProcess::Start()
    {
        NTB_TRACE_INFO_WITH_THIS("Start asyc service command process.");
        return this->activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED);
    }

    void AsycServiceCmdProcess::Stop()
    {
        this->wait();
    }

    int AsycServiceCmdProcess::close( u_long flags /*= 0*/ )
    {
        // close is hook function when thread exit, here i can do crazy thing
        // such as delete this.
        ACE_UNUSED_ARG( flags );
        m_pTcpSvr->UpdateFinishedSvr(this);

        //delete this;

        return McsfSuccess;
    }

    std::string AsycServiceCmdProcess::Dump()
    {
        MCSF_TRY
            MCSF_DUMP(m_sDumpInfo, "(void)*", "this: ", this );
            MCSF_DUMP(m_sDumpInfo, "(McsfRpcMessage)*", "m_pRequest: ", m_pRequest );
            MCSF_DUMP(m_sDumpInfo, "(TcpChannel)*", "m_pChannal: ", m_pChannal);
            MCSF_DUMP(m_sDumpInfo, "(IServerProcessCB)*", "m_pCb: ", m_pCb);
            return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
            return ""; 
        MCSF_TRY_CATCH_END
    }
}

