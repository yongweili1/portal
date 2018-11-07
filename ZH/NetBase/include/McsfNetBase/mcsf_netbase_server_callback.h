////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_netbase_server_callback.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/07
///
/// \brief   
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_NETBASE_SERVER_CALLBACK_H_
#define MCSF_NETBASE_SERVER_CALLBACK_H_

#include "mcsf_rpc_msg_redef.h"
namespace Mcsf 
{
    //////////////////////////////////////////////////////////////////////////
    // McsfNetBase Inner use interface, consumer need not take care.
    class TcpChannel;
    class AsycServiceCmdProcess;
    class IServerProcessCB 
    {
    public:
        enum 
        {
            Success = 0,
            Not_Send_Response = 1,
            Not_Handler = 2,
            Have_Exception =3,
        };

        virtual ~IServerProcessCB() {};

        virtual int HandleRpcCommand(RpcMsgPackPtr ptrRequest,
            std::string *pResponse, TcpChannel* pChannel) = 0;

        virtual int HandleRpcEvent(RpcMsgPackPtr ptrRequest) = 0;
    };

    class IServer
    {
    public:
        virtual ~IServer() {};

        virtual int Init(int iPoolSize = 1) = 0;
        virtual int Fini() = 0;

        virtual std::string GetListenAddr() = 0;

        virtual int RegisterCallBack(IServerProcessCB* pCB) = 0;
    };
}

#endif // MCSF_NETBASE_SERVER_CALLBACK_H_
