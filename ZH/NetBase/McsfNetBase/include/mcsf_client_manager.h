////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_client_manager.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/01
///
/// \brief  the socket cliant manage container
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_CLIENTMANAGE_H_
#define _MCSF_CLIENTMANAGE_H_

#include <string>
#include <map>
#include "ace/Thread_Mutex.h"
#include "ace/RW_Thread_Mutex.h"

#include "mcsf_tcp_client.h"
#include "boost/thread.hpp"

#include "mcsf_rpc_msg_redef.h"

namespace Mcsf {
    
    typedef std::map<std::string, TcpClient_Ptr> Name2Client; 
    typedef std::map<TcpChannel*, TcpHandler*> Channel2Handler;
    
     /// \class ClientManager
     /// \brief the tcp socket client container 
    class ClientManager : public IDumper
    {
    public:
        ClientManager(ACE_Reactor* r);
        ~ClientManager();

        int GetSize();
        /// determine whether a connection is establish to containee, not thread safe 
        bool IsClientRegister( const std::string& sContaineeName ); 
        /// determine whether i and containee to connected is same machine, not thread safe
        bool IsLocalHost( const std::string& sContaineeName );
        /// unregister an client by containee name
        void UnRegisterClient( const std::string& sContaineeName ); 
        /// unregister an client by underly TCP channel
        void UnRegisterClient( const TcpClient_Ptr pClient );
        /// create an TCP channel to \a sIpAddr, and register it with \a sContaineeName
        int CreateClient(const std::string& sContaineeName, const std::string& sIpAddr, bool bRetry = true);
        /// send an request to remote \a sContaineeName, 
        /// 
        /// \param   sContaineeName    the receiver name     
        /// \param   rpcMsg        *   the request rpc message 
        /// \return  CommandCall*      an command call to do async or sync command call
        CommandCallPtr SendRequest( const CommandContext* pContext, 
                    bool bNeedCallback = false, ICommandCallbackHandler* pCallback=NULL,IDataSender* pSendDer = NULL);

        /// send an request that don't need response to remote \a sContaineeName
        /// 
        /// \param   sContaineeName      the receiver name    
        /// \param   rpcMsg        *     the request rpc message
        /// \return  void 
        void SendRequestWithoutRespond( RpcMsgPackage *pck, const std::string &sReceiver );

        void SetCallBack(EVENTTYPE eEvtType, boost::function1<void,void*> ufpc);
        void CancelCallBack(EVENTTYPE eEvtType);
        void CancelAllCallback();
        void TrigReconnection();
        void AddSvrTcpHandler(TcpChannel* pChannel, TcpHandler* pHandler);
        void RemoveServer(TcpChannel* pChannel);
        ACE_Reactor* GetReactor();
        std::string Dump();
        int GetOneUid();
        std::string GetReceiverIp( const std::string &sReceiver );
        boost::shared_ptr<AsyncCallBackThread> GetAysnClientActiveObj() { return m_pAsynCallbackThread; }
        void StopClientCallBackHandlerThread() { m_pAsynCallbackThread->Stop(); }
    private:
        /// register an client channel with containee name
        void RegisterClient( const std::string& sContaineeName, 
            TcpClient_Ptr pClient );
        /// close all client TCP channel
        void DestroyAllClient();
        /// close all server TCP channel
        void RemoveAllServer();
    private:
        Name2Client m_mapClients;
        TcpClient_Ptr m_pClient;
        ACE_Thread_Mutex m_idMutex;
        int m_iIdGenerator;
        ACE_RW_Thread_Mutex m_clientMapMutex; 
        boost::mutex m_clientMutex;
        boost::function1<void,void*> m_ufpc;
        boost::mutex m_cbMutex;
        Channel2Handler m_mapChannel2Handler;
        boost::mutex m_mapServerMutex;
        ACE_Reactor* m_pReactor;
        boost::shared_ptr<AsyncCallBackThread> m_pAsynCallbackThread;
    };

    extern const std::string sSystemDispatchName;
}

#endif
