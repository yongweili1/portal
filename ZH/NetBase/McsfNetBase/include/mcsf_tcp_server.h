//==========================================================================
/**
 *  Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
 *  All rights reserved
 *
 *  @file   mcsf_tcp_server.h
 *  @author Ruifei.liu <ruifei.liu@united-imaging.com>
 * 
 *  @date   2011/09/04
 *
 *  @brief  listen on specify port and process rpc call. 
 */
//==========================================================================

#ifndef _MCSF_TCPSERVER_H_
#define _MCSF_TCPSERVER_H_

#include <string>
#include <ace/Task.h>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

#include "mcsf_acceptor.h"
#include "mcsf_netbase_interface.h"
#include "mcsf_ichannel.h"


namespace Mcsf {
    typedef std::map<EVENTTYPE, boost::function1<void, void*> > EVENTTYPE2CALLBACKMAP;
    class TcpChannel;
    class ClientManager;
    class AsycServiceCmdProcess;
    class TcpServer : public ACE_Task<ACE_MT_SYNCH>
                    , public boost::enable_shared_from_this<TcpServer>
                    , public IDumper
    {
    public:
        TcpServer( ClientManager* pClientMgr);
        ~TcpServer();

        /// set listen address
        void SetListenAddr( const std::string& sIpAddr );
        /// activate the process thread
        virtual int Init(TcpServer_Ptr pServer, int iPoolSize = 1);
        /// hook method of daemon thread
        virtual int svc();
        /// send an quit msg and wait for thread exit
        virtual int Fini();
        /// register server callback function
        virtual int RegisterCallBack(IServerProcessCB* pCB);
        /// get the listen addr
        virtual std::string GetListenAddr(); 
        /// get the listen port
        virtual std::string GetListenPort();
        /// get IP
        static std::string GetIpInterface();

        static std::string GetMacInterface();

        void SetCallBack(EVENTTYPE eEvtType, boost::function1<void,void*> ufpc);

        void CancelCallBack(EVENTTYPE eEvtType);

        void UnloadContaineeInDispt(TcpChannel* pChannel);

        void RemoveServer(TcpChannel* pChannel);

        void UpdateFinishedSvr( AsycServiceCmdProcess *pAsyncSvrCmdProcess );

        void ClearAllFinishedAsyncSvrCmdProcess();

        std::string Dump();

    private:

        void PreHandle(const std::string &sender, TcpChannel* pTcp);

        void LoopEraseFinishedSvr();

        void ClearFinishedAsyncSvrCmdProcess();

        void AddAsyncSvrCmdProcess( AsycServiceCmdProcess *pAsyncSvrCmdProcess );

    private:
        typedef std::map<TcpChannel*, std::pair<std::string, std::string>> map2pair;

        ACE_INET_Addr m_ListenAddr;         /*< address listen on*/
        Acceptor* m_pAcceptor;
        IServerProcessCB* m_pCB;            /*< callback function when receive request*/
        map2pair m_mapChannel2Name;
        EVENTTYPE2CALLBACKMAP m_fpc;
        boost::mutex m_mxMap;
        boost::mutex m_cbMutex;
        ClientManager* m_pClientMgr;

        boost::mutex m_svrMapMutex;
        std::map<AsycServiceCmdProcess*, bool> m_mapAsynSvrCmd;
        boost::condition_variable_any m_svrCond;

    };

}

#endif
