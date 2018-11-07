////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_tcp_client.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/05
///
/// \brief  represent tcp client 
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_TCPCLIENT_H_
#define _MCSF_TCPCLIENT_H_

#include <ace/Task.h>
#include <boost/enable_shared_from_this.hpp>

#include "mcsf_connector.h"
#include "mcsf_tcp_channel.h"
#include "mcsf_pending_call.h"
#include "mcsf_blobdata_interface.h"

#include "mcsf_rpc_msg_redef.h"

namespace Mcsf {

    class AsyncBigDataCallBackHandler : public ICommandCallbackHandler
    {
    public:
        explicit AsyncBigDataCallBackHandler( 
            boost::shared_ptr<IDataSender> pSender, 
            boost::shared_ptr<ICommandCallbackHandlerEx> pCallback);
        virtual int HandleReply(const IAsyncResult* pResult);
    private:
        boost::shared_ptr<IDataSender> m_pFileSender; 
        boost::shared_ptr<ICommandCallbackHandlerEx> m_pCallback;
    private: 
        bool m_bTimeOut;
    };

    class CommandCall;
    class ClientManager;
    
     ///  \class TcpClient 
     ///  \brief a tcp client with an underly thread 
    class TcpClient : public ACE_Task<ACE_MT_SYNCH>
                    , public boost::enable_shared_from_this<TcpClient>
                    , public IDumper
    {
        friend class ClientManager;
        friend class ClientTcpChannel;

        typedef std::map<int, CommandCallPtr> CommandCallMap;
        typedef std::map<int, AsyncCallTimeout*> AsyncCallTimeoutMap;

    public:
        TcpClient( ClientManager* pManage );
        ~TcpClient();

        /// Send an Request message 
        /// 
        /// \param   rpcMsg           rpc message send by socket
        /// \return  CommandCall*     a CommandCall object that will be invoke when receive 
        ///        *                  response
        CommandCallPtr SendRequest(int uid, const CommandContext *pContext,
            bool bNeedCallback = false, ICommandCallbackHandler* pCallback=NULL, IDataSender *pSender=NULL);
        /// process response message
        void ReceiveResponse( RpcMsgPackPtr resultMsg );
        /// just send request message , not expected any response
        int SendRequestWithOutResponse( RpcMsgPackage *pck );

        int Init( const std::string& sRemoteAddr, const std::string& sOpposite, bool bRetry );

        int StartThread();

        /// daemon thread start function
        virtual int svc(void);  
        ACE_INET_Addr GetNetAddr() const 
        {
            return m_NetAddr; 
        }

        virtual int close(u_long flags = 0);

        void CancelAllCallback();

        void RemovePendingCall(CommandCallPtr pCmdCall);

        void RemoveTimeoutCall(AsyncCallTimeout* pCallTimeout);

        void ClearDataFile();

        std::string Dump();
        ClientManager* GetClientManager()
        {
            return m_pManager;
        }

        std::string GetRemoteIP() const { return m_sRemoteAddr; }

    private:
        void StopThread();
        int SendVeriInfo();
        std::string m_sOpposite;
        Connector m_Connector;
        std::string m_sRemoteAddr;
        ACE_INET_Addr m_NetAddr;
        TcpHandler* m_pTcpHandler;
        ClientManager* m_pManager;
        CommandCallMap m_mapPendingCalls;       /// a map store command call object 
        AsyncCallTimeoutMap m_mapTimeoutCalls;  /// a map store async timeout call object
        ACE_Thread_Mutex m_mapMutex;
        TcpClient_Ptr m_pClientRecord;
        bool m_flag;
    };
    extern const std::string sSystemDispatchName;
}

#endif
