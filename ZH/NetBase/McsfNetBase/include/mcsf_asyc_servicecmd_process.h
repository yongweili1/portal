////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_asyc_servicecmd_process.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/01
///
/// \brief  spawn an seperate thread to process asynchronous service request
///         dispatch
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_ASYC_SERVICECMD_PROCESS_H
#define _MCSF_ASYC_SERVICECMD_PROCESS_H

#include "ace/Task_T.h"
#include "boost/shared_ptr.hpp"
#include "mcsf_rpc_msg_redef.h"

namespace Mcsf 
{
    class TcpChannel;
    class IServerProcessCB;
    class TcpServer;
    
     /// \class AsycServiceCmdProcess
     /// \brief an thread task to process asynchronous service request dispatch
    class AsycServiceCmdProcess : ACE_Task_Base, public IDumper
    {
    public:
         
        /// constructor 
        /// 
        /// \param   pRequest    The request rpc message present by google protocol buffer     
        /// \param   pChannel    the socket channel that receive request and send response     
        /// \param   pCb         the callback that register by up layer
        /// \return   
        AsycServiceCmdProcess(RpcMsgPackPtr pRequest, 
            TcpChannel* pChannel,
            IServerProcessCB *pCb,
            TcpServer *pTcpSvr);

        //destructor
        ~AsycServiceCmdProcess();

        /// activate the underly thread
        virtual int Start();

        /// Run by a daemon thread to handle deferred processing.
        virtual int svc();

        virtual void Stop();

        virtual int close(u_long flags = 0);

        std::string Dump();

    public:
        RpcMsgPackPtr m_pRequest;         /// the request rpc message
        TcpChannel* m_pChannal;                               /// the tcp socket channel
        IServerProcessCB* m_pCb;                              /// the up layer callback 
        TcpServer* m_pTcpSvr;
    };

}

#endif
