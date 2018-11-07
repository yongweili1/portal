#include "stdafx.h"
#include "ace/OS_NS_Thread.h"

#define max_retry_connect_count 3

using boost::scoped_ptr;
using boost::shared_ptr;

namespace {

    using namespace Mcsf;
    //const std::string sSystemDispatchName = "SystemDispatcher";
    // if you are C++ program, ACE macro can auto invoke ACE::init, you this
    // module is load from C#, it's my responsibility to invoke ACE::init
    class AutoStartACE
    {
    public:
        AutoStartACE() {
            ACE::init();
        }

        ~AutoStartACE() {
            //don't invoke, because maybe crash.
            //ACE::fini();
        }
    };
    AutoStartACE g_autoStartACEGuard;


    const int INNER_CMD_DEFAULT_TIMEOUT = 5000;

    class CommandContextExImpl : public ICommandContextEx
    {
    public:
        CommandContextExImpl(void* sendChannel, int uniqueId ) : 
            m_pSendChannel(sendChannel), m_iUniqueId(uniqueId)
        {

        }

        ///get the command receiver name, generally it's your name
        virtual std::string GetReceiver() const {
            return m_sReceiver;
        }

        void SetReceiver(const std::string& sReceiver) {
            m_sReceiver = sReceiver;
        }
        ///get the command sender name
        virtual std::string GetSender() const {
            return m_sSender;
        }

        void SetSender(const std::string& sSender) {
            m_sSender = sSender;
        }
        ///get the command Id
        virtual int GetCommandId() const {
            return m_iCommandId;
        }

        void SetCommandId(int iCommandId) 
        {
            m_iCommandId = iCommandId;
        }
        ///get the serialized object send from client
        virtual std::string GetSerializeObject() const {
            return m_pSerializeObject == NULL ? "" : *m_pSerializeObject;
        }

        void SetSerializedObject(boost::shared_ptr<std::string> pSerializedObject) {
            m_pSerializeObject = pSerializedObject;
        }

        virtual ICommandContextEx* Clone() {
            return new CommandContextExImpl(*this);
        }
        virtual void Destory() {
            delete this;
        };

        virtual int Reply(const std::string& replyObject) const
        {
            NTB_TRACE_INFO_WITH_THIS("Reply() enter");
            if(m_pSendChannel == NULL)
            {
                NTB_LOG_ERROR_WITH_THIS("CommandContextExImpl::Reply m_pSendChannel is null."
                    <<" m_iUniqueId="<<m_iUniqueId<<" m_iCommandId="<<m_iCommandId
                    <<" m_sSender="<<m_sSender<<" m_sReceiver="<<m_sReceiver);
                return -1;
            }

            TcpChannel* pChannel = (TcpChannel*)m_pSendChannel;

            RpcMsgPackPtr replyMsg(new RpcMsgPackage);
            replyMsg->SetSentFields(m_iUniqueId, m_iCommandId, replyObject, true);

            NTB_TRACE_INFO_WITH_THIS("begin to send.");
            pChannel->Send( replyMsg );
            NTB_TRACE_INFO_WITH_THIS("CommandContextExImpl::Reply release channel="<<pChannel);
            pChannel->Release();
            m_pSendChannel = NULL;

            return 0;
        }
    private:
        mutable void* m_pSendChannel;         //tcp send object point
        int   m_iUniqueId;            //command unique id
        int m_iCommandId;
        std::string m_sSender;
        std::string m_sReceiver;
        boost::shared_ptr<std::string> m_pSerializeObject;
    };
}


//static ACE_THR_FUNC_RETURN ReactorThreadFun( void* arg )
//{
//    ACE_UNUSED_ARG( arg );
//    NTB_TRACE_INFO_WITH_THIS( "start reactor thread." );
//    ACE_Reactor::instance()->owner(ACE_OS::thr_self());
//    ACE_Reactor::instance()->restart(true);
//    int iRet = ACE_Reactor::instance()->run_reactor_event_loop();
//    NTB_TRACE_INFO_WITH_THIS( "end reactor thread." );
//    ACE_Reactor::instance()->reset_reactor_event_loop();
//
//    return (ACE_THR_FUNC_RETURN)iRet;
//}



namespace Mcsf {

    //////////////////////////////////////////////////////////////////////////
    // implementation

    template<class T>
    struct HandlerRef_t : public ISafeRefCount
    {
        HandlerRef_t(T* pHlr)
            : pHandler(pHlr){}

        T* pHandler;

    private:
        HandlerRef_t(){}
    };

    static const ACE_Time_Value KtvInter(0, 200*1000);
    static const int kReconnectitv = 3;
    typedef boost::shared_ptr< HandlerRef_t<ICommandHandler> > cmdHandlerPtr;
    typedef boost::shared_ptr< HandlerRef_t<IEventHandler> >   entHandlerPtr;

    //typedef boost::shared_ptr< HandlerRef_t<ICommandCallbackHandler> > cmbCallBackHandlerPtr;
    //typedef std::map<int, cmbCallBackHandlerPtr > CmdId2CallBackHandler;
    //typedef std::map<int, boost::shared_ptr<ICommandCallbackHandlerEx> > CmdId2CallBackHandlerEx;
    class ReactorThread : public ACE_Task<ACE_MT_SYNCH>
    {
    public:
        ReactorThread(void* r)
        {
            NTB_TRACE_INFO_WITH_THIS( "ReactorThread::ReactorThread()");
            m_pReactor = (ACE_Reactor*) r;
        }

        ~ReactorThread()
        {
            NTB_TRACE_INFO_WITH_THIS( "ReactorThread::~ReactorThread()");
            delete this->m_pReactor;
            //ACE_Reactor::instance((ACE_Reactor*)0);
            this->m_pReactor = 0;
            this->reactor(0);
        }

        virtual int svc()
        {
            m_pReactor->owner (ACE_OS::thr_self ());
            m_pReactor->run_reactor_event_loop();
            return 0;
        }

        bool Start(int iThreadNbr, int iPriorty)
        {
            NTB_LOG_INFO_WITH_THIS( "start reactor thread. iPriorty=" << iPriorty);
#ifndef ACE_WIN32
            if (0 != iPriorty)
            {
                if (-1 == this->activate(THR_NEW_LWP | THR_JOINABLE | THR_SCHED_RR, iThreadNbr, 0, iPriorty))
                    return false;
                return true;
            }
#endif
            if (-1 == this->activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, iThreadNbr, 0, iPriorty))
                return false;
            return true;
        }

        bool Stop()
        {
            NTB_TRACE_INFO_WITH_THIS( "stop reactor thread.");

            if (NULL != this->m_pReactor)
            {
                this->m_pReactor->end_reactor_event_loop();
                if (this->wait() == -1)
                {
                    NTB_LOG_ERROR_WITH_THIS( "Not all thread safe exit"
                        <<"last error="<<ACE_OS::last_error());
                    return false;
                }
            }
            else
            {
                NTB_LOG_ERROR_WITH_THIS( "m_pReactor is null.");
                return false;
            }
            return true;
        }
        ACE_Reactor* GetReactor()
        {
            return m_pReactor;
        }
    private:
        ACE_Reactor* m_pReactor;
    };

    class CommunicationProxy::Implement : public IServerProcessCB
    {
        typedef CommunicationProxy::IdSet IdSet;
        typedef CommunicationProxy::ChannelIdMap ChannelIdMap;

        typedef std::pair<int, int> EventKey;
        typedef std::map<int, cmdHandlerPtr > CmdId2Handler;
        typedef std::map<int, boost::shared_ptr<ICommandHandlerEx> > CmdId2HandlerEx;
        typedef std::multimap<EventKey, entHandlerPtr > EventId2Handler;
        typedef std::pair<EventId2Handler::const_iterator, EventId2Handler::const_iterator> multimapRange;
    public:
        explicit Implement(int iReactorPriority) : m_iMyChannelId(0), 
            m_pAllEventHandler(NULL),
            m_pCheckCastCmdHandler(new CheckCastCmdHandler),
            m_pEchoCmdHandler(new EchoCmdHandler()),
            //m_pClientManage( new ClientManager() ), 
            m_iServerThreadPoolSize(1), m_bAlwaySendDataBySocket(false),
            m_iReactorPriority(iReactorPriority), m_bReconnectStop(false),
            m_bReconnecting(false),
            //m_pServer(boost::make_shared<TcpServer>(m_pClientManage.get()), m_pReactorThread->GetReactor()),
            m_pReactorThread(NULL)
        {
            NTB_TRACE_INFO_WITH_THIS( "CommunicationProxy Implement Constructor.");

            StartReactorEventLoop();
            //m_pClientManage = boost::make_shared<ClientManager>(m_pReactorThread->GetReactor());
            m_pClientManage = new ClientManager(m_pReactorThread->GetReactor());
            //m_pServer = boost::make_shared<TcpServer>(m_pClientManage.get());
            m_pServer = boost::make_shared<TcpServer>(m_pClientManage);

            RegisterCommandHandler( SYSTEM_COMMAND_ID_CHECK_CAST, m_pCheckCastCmdHandler );
            RegisterCommandHandler( SYSTEM_COMMAND_ID_ECHO, m_pEchoCmdHandler);

            ACE_Utils::UUID uuid;
            ACE_Utils::UUID_Generator generater;
            generater.generate_UUID(uuid);
            m_sName = uuid.to_string()->c_str();
            m_pClientManage->SetCallBack(EVENTTYPE_RECONNECT, boost::bind(
                &CommunicationProxy::Implement::ReConnectDispt, this, _1));
        }

        virtual ~Implement();

        int AsyncSendCommand(const CommandContext* pCommandContext);
        int SyncSendCommand( const CommandContext* pCommandContext, 
            std::string& sSerializeObject );
        ISyncResult* SyncSendCommand( const CommandContext* pCommandContext);
        int SyncSendData(const SendDataContext* pSendDataContext);
        int SyncSendDataEx(const SendDataContextEx* pSendDataContext, std::string& sSerializeObject);
        ISyncResult* SyncSendDataEx(const SendDataContextEx* pSendDataContext);
        int AsyncSendData( const SendDataContext* pSendDataContext );
        int AsyncSendDataEx( const SendDataContextEx* pSendDataContext );

        int RegisterCommandHandler(int iCommandId, 
            ICommandHandler* pCommandHandler);
        int RegisterCommandHandlerEx( int iCommandId, 
            boost::shared_ptr<ICommandHandlerEx> pCommandHandler );
        void UnRegisterCommandHandler(int iCommandId);
        void UnRegisterAllCommandHandlers(void);

        int RegisterDataHandler(IDataHandler* pSendDataHandler);
        int RegisterDataHandlerEx( boost::shared_ptr<IDataHandlerEx> pSendDataHandler );
        void UnRegisterDataHandler(void);
        int RegisterEventHandler( int iChannelId, int iEventId, 
            IEventHandler* pEventHandler);
        virtual void UnRegisterEventHandler(int iChannelId, int iEventId) ;
        virtual void UnRegisterAllEventHandlers() ;
        virtual void UnRegisterAllHandlers();

        int SendEvent( const std::string& sSender, int iEventId, 
            const std::string& sEvent );
        virtual int BroadcastEvent( const std::string& sSender, int iEventId, 
            const std::string& sEvent );

        int SendSystemEvent( const std::string& sSender, int iEventId, 
            const std::string& sEvent );
        virtual int BroadcastSystemEvent( const std::string& sSender, int iEventId, 
            const std::string& sEvent );

        int SubscribeEvent( const ChannelIdMap& idMap );
        int SubscribeEvent( int iChannelId, int iEventId );
        int StartReactorEventLoop();
        int StartListener( const std::string& sIpAddr );

        int CheckCastToSystemDispatcher( const std::string& sDispatchIpAddr );

        int _CheckCastToSystemDispatcher( const std::string& sDispatchIpAddr );

        int CheckCastToRemote( const std::string& sName, 
            const std::string& sIpAddr );

        int CheckCastToName(const std::string& sName, 
            int iWaitTime );

        /// set container name
        void SetName( const std::string& sName ) {
            m_sName = sName;
        } 

        /// get container name
        std::string GetName() const {
            return m_sName;
        }

        // set ip
        void SetListenAddress(const std::string& sIP)
        {
            m_sListenAdress = sIP;
        }

        // get ip
        std::string GetListenAddress()
        {
//            NTB_TRACE_INFO_WITH_THIS("GetListenAddress enter.");
            std::string sRet;
            //size_t iPos = m_sListenAdress.find(':');
            //if (std::string::npos != iPos)
            //{
            //    sRet.assign(m_sListenAdress, 0, iPos);
            //}
            sRet = TcpServer::GetIpInterface();
//            NTB_TRACE_INFO_WITH_THIS("GetListenAddress sRet="<<sRet);
            return sRet;
        }

        std::string GetMacAddress()
        {
            return TcpServer::GetMacInterface();
        }

        /// set container channel id
        void SetSenderChannelId(int iChannelId) {
            m_iMyChannelId = iChannelId;
        }

        /// get container channel id
        int GetSenderChannelId( void ) const {
            return m_iMyChannelId;
        }

        /// shut down server, end reactor event loop, stop server listen and
        /// close all client socket connection.
        int ShutDown( void );

        //void UnRegister();

        /// used for event forwarding by system dispatcher
        virtual int DispatchEvent( const std::string& sReceiver,
            const std::string& sSender,
            int iChannelId, 
            int iEventId, const std::string& sEvent, bool bBroadcast = false );

        /// used for event forwarding by system dispatcher
        virtual int DispatchEvent( const std::string& sIpAddr, const std::string& sReceiver,
            const std::string& sSender,
            int iChannelId, 
            int iEventId, const std::string& sEvent, bool bBroadcast = false );

        std::string GetSystemDispatcherAddress() const
        {
            return m_sSystemDispatcherIpAddr;
        }

        int InitNetBase(int argc, char* argv[]);

        void RegisterCallBackFunction(EVENTTYPE eEvtType, boost::function1<void,void*> pFunction)
        {
            NTB_TRACE_INFO_WITH_THIS( "RegisterCallBackFunc enter. pFunction="<<pFunction  );
            //m_pClientManage->SetCallBack(eEvtType, pFunction) ;
            m_pServer->SetCallBack(eEvtType, pFunction);

        }
        void UnregisterCallBackFunction(EVENTTYPE eEvtType)
        {
            NTB_TRACE_INFO_WITH_THIS( "UnRegisterCallBackFunc enter." );
            //m_pClientManage->CancelCallBack(eEvtType) ;
            m_pServer->CancelCallBack(eEvtType) ;
        }

        std::string FindSimilarIPAddr( const std::vector<std::string> &result, const std::string& itSender ); 

    protected:
        int ConnectSelf();
        virtual int QueryName( const std::string& sName );
        virtual int CheckCastToRemote( const std::string& sName );

        int SendCommand( CommandContext* pContext, CommandCallPtr& pCall,
                        bool bNeedCallback = false, ICommandCallbackHandler* pCallback=NULL,IDataSender *pSendData = NULL);

        int CheckCastToName( const std::string& sName );
        
        virtual int RegisterContainer( const std::string& sName );
        virtual int HandleRpcEvent( RpcMsgPackPtr ptrRequest );
        virtual int HandleRpcCommand(RpcMsgPackPtr ptrRequest,
            std::string *pResponse, TcpChannel* pChannel);

        virtual int HandleCommand(RpcMsgPackPtr request, std::string *pResponse, int iCmdId);
        virtual int HandleCommandEx(RpcMsgPackPtr request, TcpChannel* pSendChannel, int iCmdId );

        int SyncSendData( const SendDataContextEx* pSendDataContext, 
            IDataSender *pSender, std::string& sSerializeObject );

        int CalcDataSenderAddr( const std::string &sReceiver,
            IDataSender * pSender, std::string &sSendPeerPort, std::string &sSendPeerIP );

        int AsyncSendData( const SendDataContextEx* pSendDataContext, 
            IDataSender *pSender  );

        IDataSender* CreateSender( const std::string& sReceiver );

        bool IsCommand( int id );

        bool IsCommandEx( int id );

        cmdHandlerPtr GetCmdHandler( int id );

        boost::shared_ptr<ICommandHandlerEx> GetCmdExHandler( int id );

        void ReConnectDispt(void* pStr);

        void ReConnectDispt_i();

    private:
        std::string m_sName;
        CmdId2Handler m_mapCmdHandlers;
        ACE_Thread_Mutex m_mxCmd;
        CmdId2HandlerEx m_mapCmdHandlerExs;
        ACE_Thread_Mutex m_mxCmdEx;
        EventId2Handler m_mapEventHandlers;
        ACE_Thread_Mutex m_mxEvent;
        std::string m_sSystemDispatcherIpAddr;
        int m_iMyChannelId;

        IEventHandler* m_pAllEventHandler;

        ICommandHandler* m_pCheckCastCmdHandler;
        ICommandHandler* m_pEchoCmdHandler;

        //boost::shared_ptr<ClientManager> m_pClientManage;
        ClientManager* m_pClientManage;
        ACE_Thread_Mutex m_checkCastMutex;

        static int m_iRefCount;

        std::string m_sListenAdress;

        int m_iServerThreadPoolSize;
        int m_bAlwaySendDataBySocket;
        int m_iReactorPriority;
        ChannelIdMap m_idMap;
        bool m_bReconnecting;
        bool m_bReconnectStop;
        boost::thread m_thrReconnect;
        TcpServer_Ptr m_pServer;
        ReactorThread* m_pReactorThread;
        std::map<boost::thread::id, int> m_mapThdId2CmdId;
        std::map<boost::thread::id, EventKey> m_mapThdId2EvtKey;
    };

    int CommunicationProxy::Implement::m_iRefCount = 0;

    //////////////////////////////////////////////////////////////////////////
    //

    CommunicationProxy::Implement::~Implement()
    {
        // it's not my responsibility to free and will crash in C# 
        // if delete handler object, because it can be allocate in manage heap
        NTB_TRACE_INFO_WITH_THIS("Implement::~Implement enter.m_sName="<<m_sName);
        ShutDown();

        m_mapCmdHandlers.clear();
        m_mapEventHandlers.clear();
        m_mapCmdHandlerExs.clear();
        DEL_PTR( m_pCheckCastCmdHandler );
        DEL_PTR( m_pEchoCmdHandler );
    }

    int CommunicationProxy::Implement::AsyncSendCommand( const CommandContext* pCommandContext )
    {
        NTB_TRACE_INFO_WITH_THIS("AsyncSendCommand enter.sender="<<m_sName<<" receiver="
            <<pCommandContext->sReceiver<<" cmd_id="<<pCommandContext->iCommandId);
        CommandCallPtr pCall ;
        int iRet = SendCommand((CommandContext*)pCommandContext, pCall, true);
        NTB_TRACE_INFO_WITH_THIS("AsyncSendCommand exit. iRet="<<iRet<<m_sName<<" receiver="
            <<pCommandContext->sReceiver<<" cmd_id="<<pCommandContext->iCommandId);
        return iRet;
    }

    int CommunicationProxy::Implement::SyncSendCommand( const CommandContext* pCommandContext, 
        std::string& sSerializeObject )
    {
         NTB_TRACE_INFO_WITH_THIS( "SyncSendCommand enter. "<<m_sName
             <<" send sync command to Receiver="<< pCommandContext->sReceiver
             <<" cmd_id="<< pCommandContext->iCommandId);
        CommandCallPtr pCall ;
        int iRet = SendCommand((CommandContext*)pCommandContext, pCall);
        NTB_TRACE_INFO_WITH_THIS( "iRet="<<iRet);
        if(iRet != McsfSuccess)
        {
            return iRet;
        }
        
        // NTB_TRACE_INFO_WITH_THIS( "start wait. cmd_id=" << pCommandContext->iCommandId );
         // there is not use smart point because i don't know when to delete in timeout situation
        if( pCall->Wait(pCommandContext->iWaitTime) != 0 )
        {
            NTB_LOG_ERROR_WITH_THIS("SyncSendCommand is timeout."
                <<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop<<" sender="
                <<m_sName<<" send sync command to Receiver="<< pCommandContext->sReceiver
                <<" cmd_id="<< pCommandContext->iCommandId);
            return McsfSendCommandTimeout;
            // TODO when to free pCall memory, if here free it will crash when receive 
            // response
        }
        NTB_TRACE_INFO_WITH_THIS( "end wait."<<m_sName<<" send sync command to Receiver="<< pCommandContext->sReceiver
             <<" cmd_id="<< pCommandContext->iCommandId);

        RpcMsgPackPtr pCallRtn = pCall->GetResult();
        ReceivedBuff pResData = pCallRtn->GetReceivedBuff();
        if (pResData)
            sSerializeObject.swap(*pResData);

        RpcMsgHeader *pHeader = pCallRtn->GetHeader();
        iRet = pHeader->iRsrv;

        NTB_TRACE_INFO_WITH_THIS( "iRet="<<iRet);
        return iRet;
    }

    ISyncResult* CommunicationProxy::Implement::SyncSendCommand( const CommandContext* pCommandContext )
    {
        NTB_TRACE_INFO_WITH_THIS( "SyncSendCommand return ISyncResult enter." );

        SyncResult *pInnerResult = new SyncResult();
        CHECK_NULL_PTR(pInnerResult);
        int iRet = SyncSendCommand( pCommandContext, pInnerResult->m_sResult );
        pInnerResult->SetCallResult( iRet );

        return pInnerResult;
    }


    int CommunicationProxy::Implement::SyncSendData( const SendDataContext* pSendDataContext )
    {
        NTB_TRACE_INFO_WITH_THIS( "SyncSendData enter." );

        int iRet = CheckCastToName(pSendDataContext->sReceiver);
        
        SendDataContextEx dataContextEx;
        dataContextEx.pRawData = boost::shared_array<char>(new char[pSendDataContext->iLen]);
        CHECK_NULL_PTR(dataContextEx.pRawData.get());
        memcpy(dataContextEx.pRawData.get(), pSendDataContext->pRawData, 
            pSendDataContext->iLen);
        dataContextEx.sReceiver = pSendDataContext->sReceiver;
        dataContextEx.sSender = pSendDataContext->sSender;
        dataContextEx.iWaitTime = pSendDataContext->iWaitTime;
        dataContextEx.iLen = pSendDataContext->iLen;

        std::string sSerializeObject;
        return iRet == McsfSuccess ?  SyncSendData(&dataContextEx, 
            CreateSender(pSendDataContext->sReceiver), sSerializeObject) : iRet;
    }

    int CommunicationProxy::Implement::SyncSendDataEx( const SendDataContextEx* pSendDataContext, 
        std::string& sSerializeObject)
    {
        NTB_TRACE_INFO_WITH_THIS( "SyncSendDataEx enter." );

        int iRet = CheckCastToName(pSendDataContext->sReceiver);
 
        return iRet == McsfSuccess ?  SyncSendData(pSendDataContext, 
            CreateSender(pSendDataContext->sReceiver), sSerializeObject) : iRet;
    }

    ISyncResult* CommunicationProxy::Implement::SyncSendDataEx( const SendDataContextEx* pSendDataContext )
    {
        NTB_TRACE_INFO_WITH_THIS( "SyncSendDataEx enter." );

        SyncResult *pInnerResult = new SyncResult();
        CHECK_NULL_PTR(pInnerResult);
        int iRet = SyncSendDataEx( pSendDataContext, pInnerResult->m_sResult );
        pInnerResult->SetCallResult( iRet );

        return pInnerResult;
    }


    int CommunicationProxy::Implement::AsyncSendData( const SendDataContext* pSendDataContext )
    {
        NTB_TRACE_INFO_WITH_THIS( "AsyncSendData enter." );

        int iRet = CheckCastToName(pSendDataContext->sReceiver);
        
        SendDataContextEx dataContextEx;
        dataContextEx.pRawData = boost::shared_array<char>(new char[pSendDataContext->iLen]);
        CHECK_NULL_PTR(dataContextEx.pRawData.get());
        memcpy(dataContextEx.pRawData.get(), pSendDataContext->pRawData, 
            pSendDataContext->iLen);
        dataContextEx.sReceiver = pSendDataContext->sReceiver;
        dataContextEx.sSender = pSendDataContext->sSender;
        dataContextEx.iWaitTime = pSendDataContext->iWaitTime;
        dataContextEx.iLen = pSendDataContext->iLen;

        return iRet == McsfSuccess ?  AsyncSendData(&dataContextEx, 
            CreateSender(pSendDataContext->sReceiver)) : iRet;
    }

    int CommunicationProxy::Implement::AsyncSendDataEx( const SendDataContextEx* pSendDataContext )
    {
        NTB_TRACE_INFO_WITH_THIS( "AsyncSendDataEx enter." );

        int iRet = CheckCastToName(pSendDataContext->sReceiver);

        return iRet == McsfSuccess ?  AsyncSendData(pSendDataContext, 
            CreateSender(pSendDataContext->sReceiver)) : iRet;
    }


    int CommunicationProxy::Implement::RegisterCommandHandler( int iCommandId,
        ICommandHandler* pCommandHandler )
    {
        NTB_TRACE_INFO_WITH_THIS( "RegisterCommandHandler enter.cmd_id="<<iCommandId 
                        <<" pCommandHandler="<<pCommandHandler);

        if(pCommandHandler == NULL)
        {
            assert(true);
            return McsfUndefineError;
        }

        {
            ACE_GUARD_RETURN( ACE_Thread_Mutex, localmx, m_mxCmd, McsfUndefineError );
            m_mapCmdHandlers[iCommandId] = cmdHandlerPtr(new HandlerRef_t<ICommandHandler>(pCommandHandler));
        }
        

        return McsfSuccess;
    }

    int CommunicationProxy::Implement::RegisterCommandHandlerEx( int iCommandId, 
        boost::shared_ptr<ICommandHandlerEx> pCommandHandler  )
    {
        NTB_TRACE_INFO_WITH_THIS( "RegisterCommandHandlerEx enter.cmd_id="<<iCommandId 
                        <<" pCommandHandler="<<pCommandHandler);

        {
            ACE_GUARD_RETURN( ACE_Thread_Mutex, localmx, m_mxCmdEx, McsfUndefineError );
            m_mapCmdHandlerExs[iCommandId] = pCommandHandler;
        }

        return McsfSuccess;
    }

    int CommunicationProxy::Implement::RegisterDataHandler( IDataHandler* pSendDataHandler )
    {
        NTB_TRACE_INFO_WITH_THIS( "RegisterDataHandler enter. pSendDataHandler="<<pSendDataHandler);

        if( pSendDataHandler == NULL )
        {
            NTB_LOG_ERROR_WITH_THIS( "pointer can't be null." );
            return McsfUndefineError;
        }

        RegisterCommandHandlerEx( SYSTEM_COMMAND_ID_SHAREMEMORY_BIGDATA_TRANSFER,
             boost::shared_ptr<ICommandHandlerEx>(new SendDataCmdHandler(pSendDataHandler)));

        return McsfSuccess;
    }

    int CommunicationProxy::Implement::RegisterDataHandlerEx( 
        boost::shared_ptr<IDataHandlerEx> pSendDataHandler )
    {
        NTB_TRACE_INFO_WITH_THIS( "RegisterDataHandlerEx enter. pSendDataHandler="<<pSendDataHandler);

        if( pSendDataHandler == NULL )
        {
            NTB_LOG_ERROR_WITH_THIS( "pointer can't be null." 
                <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
            return McsfUndefineError;
        }

        RegisterCommandHandlerEx( SYSTEM_COMMAND_ID_SHAREMEMORY_BIGDATA_TRANSFER,
            boost::shared_ptr<ICommandHandlerEx>(new SendDataCmdHandler(pSendDataHandler)) );

        return McsfSuccess;
    }

    int CommunicationProxy::Implement::RegisterEventHandler( int iChannelId, 
        int iEventId, IEventHandler* pEventHandler )
    {
        NTB_TRACE_INFO_WITH_THIS( "RegisterEventHandler enter. pEventHandler="<<pEventHandler<<" iChannelId="<<iChannelId
                                   <<" iEventId="<<iEventId);

        assert( pEventHandler != NULL );
        //register a hander care about all event, for example system dispatcher.
        if( iChannelId == -1 && iEventId == -1 && m_sName == sSystemDispatchName)
        {
            m_pAllEventHandler = pEventHandler;
            NTB_TRACE_INFO_WITH_THIS("Register all event handler.");
            return McsfSuccess;
        }
        else if ( iChannelId == -1 && iEventId == -1 )
        {
            m_pAllEventHandler = NULL;

            return McsfUndefineError;
        }

        if (m_sName != sSystemDispatchName)
        { 
            int iSub = SubscribeEvent(iChannelId, iEventId);
            if (0 != iSub)
            {
                NTB_LOG_ERROR_WITH_THIS( m_sName <<" RegisterEventHandler Subscribe Event failed."
                             << "Error Code="<<iSub<<" Error Info:"<<ErrorCodeUtil::ExplainErrorCode(iSub)
                             <<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                             <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                             <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
                return iSub;
            }
        }
        {
            ACE_GUARD_RETURN(ACE_Thread_Mutex, localmx, m_mxEvent, McsfUndefineError);
            m_mapEventHandlers.insert( std::make_pair (std::make_pair(iChannelId, iEventId),
                (entHandlerPtr)(new HandlerRef_t<IEventHandler>(pEventHandler) )) );
        }


        return McsfSuccess;
    }


    int CommunicationProxy::Implement::SendEvent( const std::string& sSender, 
        int iEventId, const std::string& sEvent )
    {
        NTB_TRACE_INFO_WITH_THIS( "SendEvent enter. sSender="<<sSender
                         <<" iEventId="<<iEventId<<" sEvent="<<sEvent);

        std::string sEventSender = sSender.empty() ? m_sName : sSender;

        return DispatchEvent( sSystemDispatchName, sEventSender,
            m_iMyChannelId, iEventId, sEvent );
    }


    int CommunicationProxy::Implement::BroadcastEvent( const std::string& sSender,
        int iEventId, const std::string& sEvent )
    {
        NTB_TRACE_INFO_WITH_THIS( "BroadcastEvent enter. sSender="<<sSender
            <<" iEventId="<<iEventId<<" sEvent="<<sEvent);

        std::string sEventSender = sSender.empty() ? m_sName : sSender;

        return DispatchEvent( sSystemDispatchName, sEventSender,
            m_iMyChannelId, iEventId, sEvent, true );
    }


    int CommunicationProxy::Implement::SubscribeEvent( int iChannelId, int iEventId )
    {
        NTB_TRACE_INFO_WITH_THIS( "SubscribeEvent enter.iChannelId="<<iChannelId<<" iEventId="<<iEventId);

        std::string sResult = "";

        McsfSubscribeEventRequest request;
        request.set_subscriber( m_sName );

        McsfSubscribeEventRequest_EventSource eSource;
        eSource.set_channel_id( iChannelId );
        eSource.add_event_ids( iEventId );
        *(request.add_event_marks()) = eSource;

        CommandContext context;
        context.iCommandId = SYSTEM_COMMAND_ID_SUBSCRIBE_EVENTS;
        context.sReceiver = sSystemDispatchName;
        context.iWaitTime = INNER_CMD_DEFAULT_TIMEOUT;
        request.SerializeToString( &context.sSerializeObject );

        // save channel id and event id to resubscribe
        m_idMap.insert(ChannelIdMap::value_type(iChannelId, IdSet())).first->second.insert(iEventId);

        return SyncSendCommand( &context, sResult );
    }

    int CommunicationProxy::Implement::SubscribeEvent( const ChannelIdMap& idMap )
    {
        NTB_TRACE_INFO_WITH_THIS( "SubscribeEvent enter.");

        if( idMap.empty() )
        {
            NTB_TRACE_INFO_WITH_THIS( "no event need to subscribe." );
            return McsfSuccess;
        }

        McsfSubscribeEventRequest request;
        request.set_subscriber( m_sName );

        ChannelIdMap::const_iterator itr = idMap.begin();
        for( ; itr != idMap.end();itr++ )
        {
            McsfSubscribeEventRequest_EventSource eSource;
            eSource.set_channel_id( itr->first );
            NTB_TRACE_INFO_WITH_THIS("channel_id="<<itr->first);
            IdSet::const_iterator inner_itr = itr->second.begin();
            for( ; inner_itr != itr->second.end(); inner_itr++ )
            {
                eSource.add_event_ids( *inner_itr );
            }
            *(request.add_event_marks()) = eSource;
        }

        std::string sResult = "";
        CommandContext context;
        context.sReceiver = sSystemDispatchName;
        context.iCommandId = SYSTEM_COMMAND_ID_SUBSCRIBE_EVENTS;
        request.SerializeToString( &context.sSerializeObject );
        context.iWaitTime = INNER_CMD_DEFAULT_TIMEOUT;

        return SyncSendCommand( &context, sResult);
    }

    int CommunicationProxy::Implement::CheckCastToSystemDispatcher( const std::string& sDispatchIpAddr )
    {
        NTB_TRACE_INFO_WITH_THIS("CheckCastToSystemDispatcher enter.sDispatchIpAddr="<<sDispatchIpAddr);
        ACE_GUARD_RETURN(ACE_Thread_Mutex, localmx, m_checkCastMutex, McsfUndefineError);
        return _CheckCastToSystemDispatcher(sDispatchIpAddr);
    }

    int CommunicationProxy::Implement::_CheckCastToSystemDispatcher( const std::string& sDispatchIpAddr )
    {
        NTB_TRACE_INFO_WITH_THIS("_CheckCastToSystemDispatcher enter.sDispatchIpAddr="<<sDispatchIpAddr);
        int iRet = 0;
        if( !m_pClientManage->IsClientRegister( sSystemDispatchName ) )
        {
            iRet = m_pClientManage->CreateClient( sSystemDispatchName, sDispatchIpAddr );
        }

        if (iRet != McsfSuccess)
        {
            NTB_LOG_ERROR_WITH_THIS( m_sName<<" connect to system dispatcher failed.iRet="<<iRet
                <<"error info:"<<ErrorCodeUtil::ExplainErrorCode(iRet) 
                <<" sDispatchIpAddr="<<sDispatchIpAddr
                <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
            return iRet;
        }
        m_sSystemDispatcherIpAddr = sDispatchIpAddr;
        NTB_TRACE_INFO_WITH_THIS("_CheckCastToSystemDispatcher succeed.");
        return iRet;
    }

    int CommunicationProxy::Implement::StartListener( const std::string& sIpAddr )
    {
        NTB_LOG_INFO_WITH_THIS("StartListener enter.sIpAddr="<<sIpAddr);

        std::string sListenAddr = sIpAddr;

        m_pServer->SetListenAddr( sListenAddr );
        m_pServer->RegisterCallBack( this );
        int iRet = m_pServer->Init(m_pServer->shared_from_this(), m_iServerThreadPoolSize);
        if( iRet != McsfSuccess )
        {
            NTB_LOG_ERROR_WITH_THIS( "Start Server at " << sListenAddr << " failed.iRet="<<iRet
                            <<"error info:"<<ErrorCodeUtil::ExplainErrorCode(iRet) 
                            <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                            <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                            <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
            return iRet;
        }

        NTB_LOG_INFO_WITH_THIS( "Start server success."<<" m_sName="<< m_sName << " with thread pool size=" << m_iServerThreadPoolSize);

        if( m_sName != sSystemDispatchName )
        {
            for (int i = 0; i < max_retry_connect_count; ++i)
            {
                iRet = RegisterContainer(m_sName);
                if (McsfSuccess != iRet)
                {
                    NTB_LOG_ERROR_WITH_THIS("register " << m_sName << " to system dispatcher failed."
                        << " m_sSystemDispatcherIpAddr=" << m_sSystemDispatcherIpAddr
                        << " m_iMyChannelId=" << m_iMyChannelId << " m_sListenAdress=" << m_sListenAdress
                        << " m_bReconnecting=" << m_bReconnecting << " m_bReconnectStop=" << m_bReconnectStop);
                }
                else
                {
                    NTB_LOG_INFO_WITH_THIS("regiseter " << m_sName << " to system dispatcher succeed.");
                    break;
                }
            }
        }
        else
        {
            m_sSystemDispatcherIpAddr = sListenAddr;
            NTB_LOG_INFO_WITH_THIS("system dispatcher listening succeed.m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr);
        }
        
        return iRet;
    }
    
    int CommunicationProxy::Implement::DispatchEvent( const std::string& sReceiver,
        const std::string& sSender, int iChannelId, int iEventId, 
        const std::string& sEvent, bool bBroadcast  )
    {
        NTB_TRACE_INFO_WITH_THIS("DispatchEvent enter."<<" bBroadcast="<<bBroadcast
                        <<" sSender="<<sSender<<" sReceiver="<<sReceiver
                        <<" iChannelId="<<iChannelId<<" iEventId="<<" event_content="<<sEvent);

        RpcMsgPackage *pckSend(new RpcMsgPackage);

        pckSend->SetSentFields(m_pClientManage->GetOneUid(),
            iEventId, sEvent, false, message_type_event, sSender, bBroadcast, iChannelId);

        m_pClientManage->SendRequestWithoutRespond( pckSend, sReceiver );
        
        return McsfSuccess;
    }

    int CommunicationProxy::Implement::DispatchEvent( const std::string& sIpAddr, const std::string& sReceiver,
        const std::string& sSender, int iChannelId, int iEventId, 
        const std::string& sEvent, bool bBroadcast  )
    {
        NTB_TRACE_INFO_WITH_THIS("DispatchEvent enter.sIpAddr="<<sIpAddr<<" bBroadcast="<<bBroadcast
            <<" sSender="<<sSender<<" sReceiver="<<sReceiver
            <<" iChannelId="<<iChannelId<<" iEventId="<<" event_content="<<sEvent);

        RpcMsgPackage *pckSend(new RpcMsgPackage);

        int uid = m_pClientManage->GetOneUid();
        pckSend->SetSentFields(uid,
            iEventId, sEvent, false, message_type_event, sSender, bBroadcast, iChannelId);

        int iRtn = CheckCastToRemote(sReceiver, sIpAddr);
        if ( McsfSuccess == iRtn)
        {
            m_pClientManage->SendRequestWithoutRespond( pckSend, sReceiver );
            NTB_TRACE_INFO_WITH_THIS("DispatchEvent new.uid="<< uid); //don't use the pckSend any more, it's hold by shared_ptr.
        }
        else
        {
            DEL_PTR(pckSend);
            NTB_LOG_ERROR_WITH_THIS("Check cast to "<<sReceiver<<" (IP="<<sIpAddr<<")"<<" failed.iRtn="<<iRtn
                             <<"error info:"<<ErrorCodeUtil::ExplainErrorCode(iRtn) 
                             <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                             <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                             <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop 
                            << " last error=" << ACE_OS::last_error());
        }
        NTB_TRACE_INFO_WITH_THIS("DispatchEvent exit.");

        return iRtn;
    }

    //////////////////////////////////////////////////////////////////////////
    // protected and private function 


    int CommunicationProxy::Implement::HandleRpcCommand(RpcMsgPackPtr ptrRequest,
        std::string *pResponse, TcpChannel* pChannel)
    {

        int iCmdId = ptrRequest->GetHeader()->id;
        if( IsCommand(iCmdId) )
        {
            return HandleCommand(ptrRequest, pResponse, iCmdId);
        }

        if( IsCommandEx(iCmdId) )
        {
            return HandleCommandEx(ptrRequest, pChannel, iCmdId);
        }

        NTB_LOG_ERROR_WITH_THIS("HandleRpcCommand failed, not command handler.cmd_id="<<iCmdId);

        return IServerProcessCB::Not_Handler;
    }

    int CommunicationProxy::Implement::HandleCommand( RpcMsgPackPtr request, std::string *pResponse, int iCmdId)
    {
        const std::string &sSender = request->GetSender();
        //NTB_TRACE_INFO_WITH_THIS("HandleCommand enter.sender="<<sSender<<" cmd_id="<<iCmdId);

        cmdHandlerPtr pCmdHlr = GetCmdHandler(iCmdId);
        MCSF_TRY
            if( pCmdHlr )
            {
                {
                    ACE_GUARD_RETURN(ACE_Thread_Mutex, localmx, m_mxCmd, -1);
                    m_mapThdId2CmdId.insert(std::map<boost::thread::id, int>::value_type(boost::this_thread::get_id(), iCmdId));
                }

                NTB_TRACE_INFO_WITH_THIS("HandleCommand pCmdHlr="<<pCmdHlr);
                CommandContext context;
                context.sReceiver = GetName();
                context.sSender = sSender;
                ReceivedBuff pData = request->GetReceivedBuff();
                if (pData)
                {
                    //TODO: copy1
                    context.sSerializeObject.swap(*pData);
                }
                context.iCommandId = iCmdId;

                if (NULL != pCmdHlr->pHandler)
                {
                    pCmdHlr->pHandler->HandleCommand( &context, pResponse );
                }

                pCmdHlr->Release();

                {
                    ACE_GUARD_RETURN(ACE_Thread_Mutex, localmx, m_mxCmd, -1);
                    m_mapThdId2CmdId.erase(boost::this_thread::get_id());
                }

                return IServerProcessCB::Success;
            }
            NTB_LOG_ERROR_WITH_THIS("HandleCommand failed. pCmdHlr is null.");
            return IServerProcessCB::Not_Handler;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("HandleCommand occurs exception."
            <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
            <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
            <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
            pCmdHlr->Release();
            {
                ACE_GUARD_RETURN(ACE_Thread_Mutex, localmx, m_mxCmd, -1);
                m_mapThdId2CmdId.erase(boost::this_thread::get_id());
            }

            return IServerProcessCB::Have_Exception;
        MCSF_TRY_CATCH_END
    }


    int CommunicationProxy::Implement::HandleCommandEx( RpcMsgPackPtr request, TcpChannel* pSendChannel, int iCmdId )
    {
        const std::string &sSender = request->GetSender();
        //NTB_TRACE_INFO_WITH_THIS("HandleCommandEx enter.sender="<<sSender<<" cmd_id="<<iCmdId);

        //now we process remote request call
         CommandContextExImpl* pCmdContextImpl = NULL;
         MCSF_TRY
            boost::shared_ptr<ICommandHandlerEx> pCmdExHlr = GetCmdExHandler(iCmdId);
            if( pCmdExHlr )
            {
                pCmdContextImpl = new CommandContextExImpl((void*)pSendChannel, request->GetHeader()->uid);
                CHECK_NULL_PTR(pCmdContextImpl);
                pCmdContextImpl->SetReceiver(GetName());
                pCmdContextImpl->SetSender(sSender);
                pCmdContextImpl->SetSerializedObject(request->GetReceivedBuff());
                pCmdContextImpl->SetCommandId(iCmdId);
                boost::shared_ptr<ICommandContextEx> pCmdContent(pCmdContextImpl);
                pCmdExHlr->HandleCommand(pCmdContent);
                return IServerProcessCB::Not_Send_Response;
            }

            NTB_LOG_ERROR_WITH_THIS("pCmdExHlr is null.");
            return IServerProcessCB::Not_Handler;
         MCSF_CATCH(Mcsf::Exception)
             NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement "<<e.what()
             <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
             <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
             <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
            //don't return Have_Exception, the channel will be destroyed 
            return IServerProcessCB::Not_Send_Response;
         MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::HandleCommandEx occurs exception."
            <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
            <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
            <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
            return IServerProcessCB::Not_Send_Response;
         MCSF_TRY_CATCH_END
    }

    int CommunicationProxy::Implement::ConnectSelf()
    {
        NTB_TRACE_INFO_WITH_THIS("ConnectSelf enter.sName="<<m_sName);
        const std::string sLocalHost="127.0.0.1";
        if (!m_pClientManage->IsClientRegister(m_sName))
        {
            std::string sIpAddr = sLocalHost + ":";
            sIpAddr += m_pServer->GetListenPort();
            m_pClientManage->CreateClient(m_sName, sIpAddr);
            NTB_TRACE_INFO_WITH_THIS("ConnectSelf sIpAddr="<<sIpAddr);
        }
        return 0;
    }

    int CommunicationProxy::Implement::QueryName( const std::string& sName )
    {
        MCSF_TRY
            NTB_TRACE_INFO_WITH_THIS("QueryName enter.sName="<<sName);
            int iRtn = 0;
            iRtn = _CheckCastToSystemDispatcher(m_sSystemDispatcherIpAddr);
            if (iRtn != McsfSuccess)
            {
                NTB_LOG_ERROR_WITH_THIS("_CheckCastToSystemDispatcher failed.m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                                        <<" iRtn="<<iRtn);
                return iRtn;
            }
            McsfQueryContaineeRequest request;
            request.set_name( sName );
        
            std::string sReponseResult("");
            CommandContext context;
            context.sSender = m_sName;
            context.sReceiver = sSystemDispatchName;
            context.iCommandId = SYSTEM_COMMAND_ID_FIND_CONTAINEE;
            request.SerializeToString( &context.sSerializeObject );
            context.iWaitTime = INNER_CMD_DEFAULT_TIMEOUT;

            CommandCallPtr pCall = m_pClientManage->SendRequest( &context );

            if(pCall == NULL)
            {
                NTB_LOG_ERROR_WITH_THIS("pCall == NULL");
                return McsfCommandCallReturnNULL;
            }

            // NTB_TRACE_INFO_WITH_THIS( "start wait. cmd_id=" << pCommandContext->iCommandId );
            // there is not use smart point because i don't know when to delete in timeout situation
            if( pCall->Wait(context.iWaitTime) != 0 )
            {
                NTB_LOG_ERROR_WITH_THIS("timeout. m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                    <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                    <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop
                    <<m_sName);
                return McsfSendCommandTimeout;
            }
            //one time memcpy 3

            McsfQueryContaineeResponse reponse;

            RpcMsgPackPtr pResponsePack = pCall->GetResult();
            ReceivedBuff buffResponse = pResponsePack->GetReceivedBuff();
            if (buffResponse)
                sReponseResult.swap(*buffResponse);
            iRtn = pResponsePack->GetHeader()->iRsrv;
            if ( iRtn != McsfSuccess)
            {
                NTB_LOG_ERROR_WITH_THIS( "Query " << sName << " command failed.iRtn = "<<iRtn
                                <<" error into:"<<ErrorCodeUtil::ExplainErrorCode(iRtn));
                return McsfQueryNameCommandFailed;
            }
            else
            {
                reponse.ParseFromString( sReponseResult );
                if(reponse.ip_addr().empty())
                {
                    NTB_LOG_ERROR_WITH_THIS( sName << " is not register in SysDispater."
                        <<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                        <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                        <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
                    return McsfContaineeNotRegistedInDispatcher;
                }
                else
                {
                    NTB_TRACE_INFO_WITH_THIS("sName="<<reponse.name()<<" ip addr="<<reponse.ip_addr());
                }
            }
            iRtn = m_pClientManage->CreateClient(reponse.name(), reponse.ip_addr() );           
            return iRtn;
            MCSF_CATCH(std::exception)         
               NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::QueryName "<<e.what()
               <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
               <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
               <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);         

            MCSF_CATCH_ALL         
               NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::QueryName occurs exception."
               <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
               <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
               <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);

            MCSF_TRY_CATCH_END                             
            return McsfUndefineError;
    }

    int CommunicationProxy::Implement::CheckCastToRemote( const std::string& sName )
    {
        MCSF_TRY
        NTB_TRACE_INFO_WITH_THIS("CheckCastToRemote enter.sName="<<sName);
        std::string sReponseResult("");
        CommandContext context;
        context.sReceiver = sName;
        context.iCommandId = SYSTEM_COMMAND_ID_CHECK_CAST;
        context.iWaitTime = INNER_CMD_DEFAULT_TIMEOUT;
        context.sSerializeObject = "";
        int iRet = this->SyncSendCommand(&context, sReponseResult);
        if( iRet == McsfSuccess )
        {
            NTB_TRACE_INFO_WITH_THIS( "connection is setup to " << sName );
        }

        return iRet;
        MCSF_CATCH(std::exception)         
            NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::CheckCastToRemote "<<e.what()
            <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
            <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
            <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);         

        MCSF_CATCH_ALL         
            NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::CheckCastToRemote occurs exception."
            <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
            <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
            <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);

        MCSF_TRY_CATCH_END    
        return McsfUndefineError;
    }

    int CommunicationProxy::Implement::CheckCastToRemote( const std::string& sName,
        const std::string& sIpAddr )
    {
        MCSF_TRY

        NTB_TRACE_INFO_WITH_THIS( "CheckCastToRemote enter. sName=" << sName<<" sIpAddr="<<sIpAddr);

        int iRet = McsfSuccess;
        if (sName.empty() || sIpAddr.empty())
        {
            iRet = McsfUndefineError;
            return iRet;
        }
        if( false == m_pClientManage->IsClientRegister( sName ) )
        {
            iRet = m_pClientManage->CreateClient(  sName, sIpAddr , false );
        }
        return iRet;

        MCSF_CATCH(std::exception)         
            NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::CheckCastToRemote"<<e.what()
            <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
            <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
            <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);         

        MCSF_CATCH_ALL         
            NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::CheckCastToRemote occurs exception."
            <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
            <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
            <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);

        MCSF_TRY_CATCH_END    
        return McsfUndefineError;
    }

    int CommunicationProxy::Implement::SendCommand( CommandContext* pContext, 
        CommandCallPtr& pCall, bool bNeedCallback, ICommandCallbackHandler* pCallback,IDataSender* pSender)
    {
        MCSF_TRY

        NTB_TRACE_INFO_WITH_THIS("SendCommand enter.sender="<<m_sName<<" receiver="<<pContext->sReceiver
                       <<" bNeedCallback="<<bNeedCallback<<" pCallback="<<pCallback<<" cmd_id="<<pContext->iCommandId);
        int iRet = 0;
        if (m_sName.compare(pContext->sReceiver))
        {
            iRet = CheckCastToName(pContext->sReceiver);
            if( iRet != McsfSuccess ) 
            {
                NTB_LOG_ERROR_WITH_THIS(m_sName<<" CheckCastTo sReceiver="<<pContext->sReceiver<<" failed.");
                return iRet;
            }
        }
        else//send command to self
        {
            iRet = ConnectSelf();
            if( iRet != McsfSuccess ) 
            {
                NTB_LOG_ERROR_WITH_THIS(m_sName<<"CheckCastTo sReceiver="<<pContext->sReceiver<<" failed.");
                return iRet;
            }
        }

        pContext->sSender = m_sName;

        pCall = m_pClientManage->SendRequest(pContext, bNeedCallback, pCallback, pSender);
        return (pCall != NULL) ? McsfSuccess : McsfCommandCallReturnNULL;

        MCSF_CATCH(std::exception)         
            NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::SendCommand "<<e.what()
            <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
            <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
            <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);         

        MCSF_CATCH_ALL         
            NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::SendCommand occurs exception."
            <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
            <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
            <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
        MCSF_TRY_CATCH_END    
        return McsfUndefineError;
    }

    int CommunicationProxy::Implement::CheckCastToName( const std::string& sName )
    {
        MCSF_TRY
            NTB_TRACE_INFO_WITH_THIS("CheckCastToName enter.sName="<<sName);
            if( false == m_pClientManage->IsClientRegister( sName ) )
            {
                ACE_GUARD_RETURN(ACE_Thread_Mutex, localmx, m_checkCastMutex, McsfUndefineError);
                if( false == m_pClientManage->IsClientRegister( sName ) )
                {
                    int iRet = McsfSuccess;
                    if( sName == sSystemDispatchName )
                    {
                        iRet = _CheckCastToSystemDispatcher( m_sSystemDispatcherIpAddr );
                        if(iRet != McsfSuccess)
                        {
                            NTB_LOG_ERROR_WITH_THIS("CheckCastToSystemDispatcher failed.m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                                <<" m_sName="<<m_sName
                                <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                                <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
                        }
                        return iRet;
                    }

                    if (m_bReconnecting) {
                        return McsfSocketConnectFailed;
                    }

                    iRet = QueryName( sName );
                    if( iRet != McsfSuccess )
                    {
                        NTB_LOG_ERROR_WITH_THIS( "Query or connect to " << sName << " failed.");
                        return iRet;
                    }
                }
            }
        MCSF_CATCH(std::exception)         
            NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::CheckCastToName "<<e.what()
            <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
            <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
            <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);         

        MCSF_CATCH_ALL         
            NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::CheckCastToName occurs exception."
            <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
            <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
            <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
        MCSF_TRY_CATCH_END    
        return McsfSuccess;
    }

    int CommunicationProxy::Implement::CheckCastToName( const std::string& sName, int iWaitTime )
    {
        NTB_TRACE_INFO_WITH_THIS("CheckCastToName enter with time.sName="<<sName);
        CommandContext context;
        context.sReceiver = sSystemDispatchName;
        context.iCommandId = SYTTEM_COMMAND_ID_CHECKCAST_TONAME;

        McsfCheckCastToNameRequest request;
        request.set_name(sName);
        request.set_timeout(iWaitTime);
        context.iWaitTime = iWaitTime;
        request.SerializeToString( &context.sSerializeObject );
        std::string sResponse;
        int iRet = SyncSendCommand( &context , sResponse) ;

        if(iRet == McsfSuccess)
        {
            McsfCheckCastToNameResponse response;
            response.ParseFromString(sResponse);
            if(response.result() != 0)
            {
                NTB_LOG_ERROR_WITH_THIS("CheckCastToName response error.");
                return McsfUndefineError;
            }
        }
        return iRet;
    }

    void CommunicationProxy::Implement::ReConnectDispt_i()
    {
        NTB_LOG_INFO_WITH_THIS("reconnect to dispatcher.m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
            <<" m_bReconnectStop="<<m_bReconnectStop);
        while (!m_bReconnectStop)
        {
            if (McsfSuccess == this->CheckCastToSystemDispatcher(m_sSystemDispatcherIpAddr))
            {
                //ACE_DEBUG((LM_DEBUG, "check this (%s) between dispatcher is connected. reconnect dispatcher %s...\n"
                //    , m_sName.c_str(), m_sSystemDispatcherIpAddr.c_str()));
                NTB_LOG_INFO_WITH_THIS("check this between dispatcher is connected. "<<
                    m_sName <<"reconnect dispatcher.dispatchaddr=" <<  m_sSystemDispatcherIpAddr);
                int iReg = RegisterContainer(m_sName);
                if (0 != iReg)
                {
                    //ACE_DEBUG((LM_DEBUG, "----register container %s to dispatcher failed=>%d \n" , m_sName.c_str() , iReg));
                    NTB_LOG_ERROR_WITH_THIS(m_sName << " register container to dispatcher failed.iRet. iRtn=" << iReg
                        <<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                        <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                        <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
                    continue;
                }

                int iResub = SubscribeEvent(m_idMap);
                if (0 != iResub)
                {
                    //ACE_DEBUG((LM_DEBUG, "++++%s SubscribeEvent %d to dispatcher failed \n", m_sName.c_str() ,m_idMap.size()));
                    NTB_LOG_ERROR_WITH_THIS(m_sName <<" SubscribeEvent to dispatcher failed " 
                        <<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                        <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                        <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
                    continue;
                }
                //ACE_DEBUG((LM_DEBUG, "****%s reconnect to dispatcher success \n" , m_sName.c_str()));
                NTB_LOG_INFO_WITH_THIS(m_sName <<" reconnect to dispatcher success " );
                break;
            }
            NTB_LOG_INFO_WITH_THIS(m_sName<<" reconnect to dispatcher failed " );

            for (int i=0; i<kReconnectitv; ++i)
            {
                if (m_bReconnectStop)
                {
                    goto over;
                }
                ACE_OS::sleep(1);
            }
        }
over:
        m_bReconnecting = false;
    }

    void CommunicationProxy::Implement::ReConnectDispt(void* pStr)
    {
        ACE_UNUSED_ARG(pStr);
        if (m_bReconnectStop || m_bReconnecting)
        {
            return;
        }

        NTB_LOG_INFO_WITH_THIS(m_sName << " try reconnect to dispatcher...");
        m_bReconnecting = true;
        boost::thread td(boost::bind(&CommunicationProxy::Implement::ReConnectDispt_i, this));
        td.detach();
    }

    int CommunicationProxy::Implement::RegisterContainer( const std::string& sName )
    {
        MCSF_TRY
            NTB_LOG_INFO_WITH_THIS("RegisterContainer enter.sName="<<sName);

            McsfRegisterContaineeRequest request;
            request.set_name( sName );
            std::string sIpAddr = m_pServer->GetListenAddr();
            if (sIpAddr.empty())
            {
                return CheckCastToSystemDispatcher(m_sSystemDispatcherIpAddr);
            }
            sIpAddr += "+" + GetMacAddress();
            request.set_ip_addr( sIpAddr );
            NTB_TRACE_INFO_WITH_THIS("RegisterContainee "<<sName<<". sIpAddr="<<sIpAddr);

            std::string sResponse;
            CommandContext context;
            context.sReceiver = sSystemDispatchName;
            context.iCommandId = SYSTEM_COMMAND_ID_REGIST_CONTAINEE;
            request.SerializeToString( &context.sSerializeObject );
            context.iWaitTime = 10000;//INNER_CMD_DEFAULT_TIMEOUT;
            int iRet = SyncSendCommand( &context , sResponse) ;
            if( iRet == McsfSuccess )
            {
                NTB_LOG_INFO_WITH_THIS( "register " << sName 
                    << "to system dispatcher success. sIpAddr="<<sIpAddr );
            }
            else
            {
                NTB_LOG_ERROR_WITH_THIS( "Register " << sName << "failed. sIpAddr="<<sIpAddr
                    <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                    <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                    <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
            }
            return iRet;
            MCSF_CATCH(std::exception)         
                NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::RegisterContainer "<<e.what()
                <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);         

            MCSF_CATCH_ALL         
                NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::RegisterContainer occurs exception."
                <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
            MCSF_TRY_CATCH_END  
            return McsfUndefineError;
    }

    int CommunicationProxy::Implement::HandleRpcEvent( RpcMsgPackPtr ptrRequest )
    {
        //now we process remote request call
        RpcMsgHeader *pHeader = ptrRequest->GetHeader();
        const uint32_t &iChannelID = pHeader->iRsrv;
        const uint32_t &iEventID = pHeader->id;
        const uint32_t &iUID = pHeader->uid;
        const std::string &sender = ptrRequest->GetSender();

        std::list<entHandlerPtr> listHandlers;
        {
            ACE_GUARD_RETURN(ACE_Thread_Mutex, localmx, m_mxEvent, McsfUndefineError);
            EventKey mapkey(iChannelID, iEventID);
            m_mapThdId2EvtKey.insert(std::map<boost::thread::id, EventKey>::value_type(boost::this_thread::get_id(), mapkey));
            multimapRange range = m_mapEventHandlers.equal_range( 
                std::make_pair( iChannelID,iEventID ));

            for(EventId2Handler::const_iterator itr = range.first; itr != range.second; ++itr )
            {
                itr->second->Duplicate();
                listHandlers.push_back(itr->second);
            }
        }

        ReceivedBuff pData = ptrRequest->GetReceivedBuff();
        BOOST_FOREACH(entHandlerPtr evtHldr, listHandlers)
        {
            try
            {
                if (NULL != evtHldr->pHandler)
                {
                    uint64_t t = GetCurrentUTCTime();

                    evtHldr->pHandler->HandleEvent(sender,
                        iChannelID,
                        iEventID, 
                        pData ? *pData : "" );

                    if (GetCurrentUTCTime() - t > 2000) {
                        NTB_LOG_INFO_WITH_THIS("long time to handle event. e=" << iEventID <<" u="<<iUID << " Sender="<<sender <<":" <<t);
                    }
                }
                else
                {
                    NTB_LOG_ERROR_WITH_THIS("evtHldr->pHandler is null."
                        <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                        <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                        <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
                }
                evtHldr->Release();
            }
            catch (...)
            {
                NTB_LOG_ERROR_WITH_THIS("HandleRpcEvent occurs exception."
                    <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                    <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                    <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
                evtHldr->Release();
                ACE_GUARD_RETURN(ACE_Thread_Mutex, localmx, m_mxEvent, McsfUndefineError);
                m_mapThdId2EvtKey.erase(boost::this_thread::get_id());
            }
        }

        if( m_pAllEventHandler != NULL )
        {
            //dispatcher.
            char buf[512] = {0};
            sprintf(buf, "%d\001%s", pHeader->bRsrv, sender.c_str());
            m_pAllEventHandler->HandleEvent(buf,
                iChannelID,
                iEventID, 
                pData ? *pData : "" );
        }

        ACE_GUARD_RETURN(ACE_Thread_Mutex, localmx, m_mxEvent, McsfUndefineError);
        m_mapThdId2EvtKey.erase(boost::this_thread::get_id());

        return McsfSuccess;
    }

    std::string CommunicationProxy::Implement::FindSimilarIPAddr( const std::vector<std::string> &result, const std::string& itSender ) 
    {
        std::string sRtn;
        size_t iIndex = 0;
        iIndex = itSender.find_last_of(".");
        if (std::string::npos == iIndex)
        {
            return "";
        }
        std::string sSubSender = itSender.substr(0,iIndex);
        for (; !sSubSender.empty();)
        {
            for (std::vector<std::string>::const_iterator it = result.begin(); it != result.end(); ++it)
            {
                if (it->compare("127.0.0.1") && !it->compare(0,sSubSender.size(),sSubSender))
                {
                    sRtn += *it;
                    sRtn += "&";
                }
            } 

            if (!sRtn.empty())
                return sRtn.substr(0, sRtn.size() - 1);

            iIndex = sSubSender.find_last_of(".");
            if (iIndex != std::string::npos)
            {
                sSubSender=sSubSender.substr(0,iIndex);
            }
            else
            {
                sSubSender.clear();
            }
        }
        return "";
    }


    int CommunicationProxy::Implement::SyncSendData( const SendDataContextEx* pSendDataContext,
        IDataSender *pSender, std::string& sSerializeObject )
    {
        MCSF_TRY

            NTB_TRACE_INFO_WITH_THIS("SyncSendData enter.sender="<<pSendDataContext->sSender
                           <<" receiver="<<pSendDataContext->sReceiver);

            if( pSendDataContext->pRawData.get() == NULL 
                || pSendDataContext->iLen <= 0 )
            {
                return McsfUndefineError;
            } 

            pSender->SetTimeOut( pSendDataContext->iWaitTime );
            if( false == pSender->SendRawData( pSendDataContext->pRawData, 
                pSendDataContext->iLen ) )
            {
                return McsfSendDataFailed;
            }
            std::string sSendPeerIP;
            std::string sSendPeerPort;
            if (pSender->GetSendType() == McsfSendDataRequest_SendType_SOCKET)
            {
                if (CalcDataSenderAddr(pSendDataContext->sReceiver, pSender, sSendPeerPort, sSendPeerIP) == 0)
                {
                    sSendPeerIP = sSendPeerIP+":"+sSendPeerPort;
                }
                else
                {
                    sSendPeerIP = pSender->GetName();
                }
            }
            else
            {
                sSendPeerIP = pSender->GetName();
            }

            //NTB_LOG_INFO_WITH_THIS("Sender addr="<<sSendPeerIP);
            McsfSendDataRequest request;
            request.set_name( sSendPeerIP ); 
            request.set_type( pSender->GetSendType() );
            request.set_is_sync(true);
            request.set_content(pSendDataContext->sSerailizedObject);

            CommandContext context;
            context.sReceiver = pSendDataContext->sReceiver;
            context.iCommandId = SYSTEM_COMMAND_ID_SHAREMEMORY_BIGDATA_TRANSFER;
            request.SerializeToString( &context.sSerializeObject );

            CommandCallPtr pCall;
            int iRet = SendCommand(&context, pCall, false , NULL, pSender);
            if(iRet != McsfSuccess)
            {
                if (iRet == McsfSocketConnectFailed)
                {
                    pSender->DestoryRawData();
                }
                return iRet;
            }

            if( pCall->Wait( pSendDataContext->iWaitTime ) == -1 )
            {
                NTB_LOG_ERROR_WITH_THIS("SyncSendData timeout.");
                return McsfSendCommandTimeout;
                // TODO when to free pCall memory, if here free it will crash when receive 
                // response
            }

            ReceivedBuff pby = pCall->GetResult()->GetReceivedBuff();
            if (!pby)
                return McsfSendDataFailed;

            McsfSendDataResponse response;
            response.ParseFromString( *pby ); 
            if(response.has_contents())
            {
                sSerializeObject = response.contents();
            }

            return response.is_success() ? McsfSuccess : McsfResponseSayFailed;

            MCSF_CATCH(std::exception)         
                NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::RegisterContainer "<<e.what()
                <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);         

            MCSF_CATCH_ALL         
                NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::RegisterContainer occurs exception."
                <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
            MCSF_TRY_CATCH_END  
            return McsfUndefineError;
    }

    int CommunicationProxy::Implement::AsyncSendData( const SendDataContextEx* pSendDataContext, 
        IDataSender *pSender )
    {
        NTB_TRACE_INFO_WITH_THIS("AsyncSendData enter.sender="<<pSendDataContext->sSender
            <<" receiver="<<pSendDataContext->sReceiver);

        //use shared_ptr to ensure the release of memory
        boost::shared_ptr<IDataSender> pSendGuard( pSender );

        if( pSendDataContext->pRawData.get() == NULL 
            || pSendDataContext->iLen <= 0 )
        {
            NTB_LOG_ERROR_WITH_THIS("pSendDataContext->pRawData.get() is NULL or pSendDataContext->iLen <= 0!!"
                <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
            return McsfUndefineError;
        }

        // less than zero to indicate asynchronous data send 
        if( false == pSendGuard->SendRawData( pSendDataContext->pRawData, 
            pSendDataContext->iLen ) )
        {
            NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::AsyncSendData: SendRawData false!!"
                <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
                <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
                <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
            return McsfSendDataFailed;
        }

        std::string sSendPeerIP;
        std::string sSendPeerPort;
        if (pSender->GetSendType() == McsfSendDataRequest_SendType_SOCKET)
        {
            if (CalcDataSenderAddr(pSendDataContext->sReceiver, pSendGuard.get(), sSendPeerPort, sSendPeerIP) == 0)
            {
                sSendPeerIP = sSendPeerIP+":"+sSendPeerPort;
            }
            else
            {
                sSendPeerIP = pSendGuard->GetName();
            }
        }
        else
        {
            sSendPeerIP = pSendGuard->GetName();
        }

        NTB_TRACE_INFO_WITH_THIS("Sender addr="<<sSendPeerIP);

        McsfSendDataRequest request;
        request.set_name( sSendPeerIP ); 
        request.set_type( pSendGuard->GetSendType() );
        request.set_is_sync(false);
        request.set_content(pSendDataContext->sSerailizedObject);
        NTB_TRACE_INFO_WITH_THIS("file name="<<request.name() );
        CommandContext context;
        context.iWaitTime = pSendDataContext->iWaitTime;
        context.sReceiver = pSendDataContext->sReceiver;
        context.iCommandId = SYSTEM_COMMAND_ID_SHAREMEMORY_BIGDATA_TRANSFER;
        request.SerializeToString( &context.sSerializeObject );

        CommandCallPtr pCall;
        int iRet = SendCommand( &context, pCall,true, new AsyncBigDataCallBackHandler(pSendGuard, 
            pSendDataContext->pCallback));

        if (McsfSuccess != iRet)
        {
            NTB_LOG_INFO("CommunicationProxy::Implement::AsyncSendData: SendCommand is not success!!");
            pSendGuard->DestoryRawData();
        }
        return iRet;
    }

    IDataSender* CommunicationProxy::Implement::CreateSender( const std::string& sReceiver )
    {
        IDataSender* pSender = NULL;
        MCSF_TRY
            NTB_TRACE_INFO_WITH_THIS("CreateSender enter.sRecevier="<<sReceiver);            
            if( m_pClientManage->IsLocalHost( sReceiver ) && !m_bAlwaySendDataBySocket )
            {
                pSender = new SMFileSender();
                if (NULL == pSender)
                {
                    MCSF_THROW_WITH_ERROR_CODE(NullPointerException,
                        ERROR_MCSF_NULL_POINTER_EXCEPTION,
                        "pSender is nullptr");
                }
            }
            else 
            {
                pSender = new SocketSender();
                CHECK_NULL_PTR(pSender);
                pSender->SetName("");
            }
            
        MCSF_CATCH(std::exception)         
            NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::CreateSender "<<e.what()
            <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
            <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
            <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);         
            return NULL;
        MCSF_CATCH_ALL         
            NTB_LOG_ERROR_WITH_THIS("CommunicationProxy::Implement::CreateSender occurs exception."
            <<" m_sName="<<m_sName<<" m_sSystemDispatcherIpAddr="<<m_sSystemDispatcherIpAddr
            <<" m_iMyChannelId="<<m_iMyChannelId<<" m_sListenAdress="<<m_sListenAdress
            <<" m_bReconnecting="<<m_bReconnecting<<" m_bReconnectStop="<<m_bReconnectStop);
            return NULL;
        MCSF_TRY_CATCH_END 
            return pSender;
    }

    int CommunicationProxy::Implement::StartReactorEventLoop()
    {
        NTB_TRACE_INFO_WITH_THIS("StartReactorEventLoop enter." << m_iRefCount);
        MCSF_TRY
            ACE_Reactor* pReactor = NULL;
#ifdef ACE_WIN32
        ACE_Select_Reactor* pSelectReactor = new ACE_Select_Reactor();
        CHECK_NULL_PTR(pSelectReactor);
        pReactor = new ACE_Reactor(pSelectReactor, true);
#else
        // here is a bug in ACE default select reactor in 6.0.1 version under debian environment ?
        ACE_Dev_Poll_Reactor* pPollReactor = new ACE_Dev_Poll_Reactor();
        CHECK_NULL_PTR(pPollReactor);
        pReactor = new ACE_Reactor(pPollReactor, true);
#endif

        CHECK_NULL_PTR(pReactor);
        // ACE_Reactor::instance(pReactor);
        m_pReactorThread = new ReactorThread(pReactor);
        CHECK_NULL_PTR(m_pReactorThread);
        if (!m_pReactorThread->Start(1, m_iReactorPriority))
        {
            NTB_LOG_ERROR_WITH_THIS("start reactor loop error!");
        }

        MCSF_CATCH(std::exception)
            NTB_LOG_ERROR_WITH_THIS("Exception occrus " << e.what()
                << " m_sName=" << m_sName << " m_sSystemDispatcherIpAddr=" << m_sSystemDispatcherIpAddr
                << " m_iMyChannelId=" << m_iMyChannelId << " m_sListenAdress=" << m_sListenAdress
                << " m_bReconnecting=" << m_bReconnecting << " m_bReconnectStop=" << m_bReconnectStop);
        return McsfUndefineError;

        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Exception occrus."
                << " m_sName=" << m_sName << " m_sSystemDispatcherIpAddr=" << m_sSystemDispatcherIpAddr
                << " m_iMyChannelId=" << m_iMyChannelId << " m_sListenAdress=" << m_sListenAdress
                << " m_bReconnecting=" << m_bReconnecting << " m_bReconnectStop=" << m_bReconnectStop);
        return McsfUndefineError;
        MCSF_TRY_CATCH_END
            return McsfSuccess;
    }

    int CommunicationProxy::Implement::ShutDown( void )
    {
        NTB_TRACE_INFO_WITH_THIS("ShutDown enter.");
        m_bReconnectStop = true;
        m_pReactorThread->Stop();
        if( m_pServer != NULL )
        {
            m_pServer->Fini();
            NTB_TRACE_INFO_WITH_THIS("Finish server tread successfully.");
        }

        m_pClientManage->StopClientCallBackHandlerThread();

        UnRegisterAllHandlers();

        while(m_bReconnecting)
        {
            ACE_OS::sleep(1);
        }
        DEL_PTR(m_pClientManage);
        DEL_PTR(m_pReactorThread);
        NTB_TRACE_INFO_WITH_THIS( "Reactor thread exits.");

        return McsfSuccess;
    }

    int CommunicationProxy::Implement::InitNetBase( int argc, char* argv[] )
    {
        NTB_TRACE_INFO_WITH_THIS("InitNetBase enter.");

        static const char options[] = ":f:";
        ACE_Get_Opt cmdOpts( argc, argv, options );
        cmdOpts.long_option( "tp_size", 0, ACE_Get_Opt::ARG_REQUIRED );
        cmdOpts.long_option( "always_socket", 1, ACE_Get_Opt::NO_ARG );
        cmdOpts.long_option( "start_telnet", 2, ACE_Get_Opt::ARG_REQUIRED);
        int iOption = 0;

        while ( ( iOption = cmdOpts() ) != EOF )
        {
            switch ( iOption )
            {
            case 0:
                {
                    try 
                    {
                        m_iServerThreadPoolSize = boost::lexical_cast<int>(cmdOpts.opt_arg());  
                    }
                    catch(boost::bad_lexical_cast)
                    {
                        m_iServerThreadPoolSize = 1;
                    }
                    break;
                }
            case 1:
                {
                    m_bAlwaySendDataBySocket = true;
                    break;
                }
            case ':':
                {
                    printf("-%c requires an argument\n", (char)cmdOpts.opt_opt());
                    return -1;
                }
            default:
                break;
            }
        }
        return 0;
    }

    int CommunicationProxy::Implement::SendSystemEvent( const std::string& sSender, 
        int iEventId, const std::string& sEvent )
    {
        NTB_TRACE_INFO_WITH_THIS("SendSystemEvent enter. sender="<<sSender);
        std::string sEventSender = sSender.empty() ? m_sName : sSender;

        return DispatchEvent( sSystemDispatchName, sEventSender,
            0, iEventId, sEvent );
    }

    int CommunicationProxy::Implement::BroadcastSystemEvent( const std::string& sSender, 
        int iEventId, const std::string& sEvent )
    {
        NTB_TRACE_INFO_WITH_THIS("BroadcastSystemEvent enter. sender="<<sSender);

        std::string sEventSender = sSender.empty() ? m_sName : sSender;

        return DispatchEvent( sSystemDispatchName, sEventSender,
            0, iEventId, sEvent, true );
    }

    void CommunicationProxy::Implement::UnRegisterCommandHandler( int iCommandId )
    {
        NTB_TRACE_INFO_WITH_THIS("UnRegisterCommandHandler enter. iCommandId="<<iCommandId);
        {
            ACE_GUARD(ACE_Thread_Mutex, localmx, m_mxCmd);
            cmdHandlerPtr ptr;
            BOOST_AUTO(it, m_mapCmdHandlers.find(iCommandId));
            if (it != m_mapCmdHandlers.end())
            {
                ptr = it->second;
            }
            int iCount = 1;
            if (ptr)
            {
                std::map<boost::thread::id, int>::const_iterator itMapTid2CmdId = m_mapThdId2CmdId.find(boost::this_thread::get_id());
                if (itMapTid2CmdId != m_mapThdId2CmdId.end() && itMapTid2CmdId->second == iCommandId)
                {
                    iCount = 2;
                }
                while(ptr->GetCount() > iCount)
                {
                    ACE_OS::sleep(KtvInter);
                }
                m_mapCmdHandlers.erase(iCommandId);
            }
        }

        {
            ACE_GUARD(ACE_Thread_Mutex, localmx, m_mxCmdEx);
            m_mapCmdHandlerExs.erase(iCommandId);
        }
    }

    void CommunicationProxy::Implement::UnRegisterAllCommandHandlers( void )
    {
        NTB_TRACE_INFO_WITH_THIS("Unregister all handlers (command handlers and command callback handlers)." );
        {
            ACE_GUARD(ACE_Thread_Mutex, localmx, m_mxCmd);
            std::map<boost::thread::id, int>::const_iterator itMapTid2CmdId = m_mapThdId2CmdId.find(boost::this_thread::get_id());
            for (CmdId2Handler::iterator it = m_mapCmdHandlers.begin(); it != m_mapCmdHandlers.end(); ++it)
            {
                int iCount = 1;
                cmdHandlerPtr tempPtr = it->second;
                if (tempPtr)
                {
                    if (itMapTid2CmdId != m_mapThdId2CmdId.end() && it->first == itMapTid2CmdId->second)
                    {
                        iCount = 2;
                    }
                    while(tempPtr->GetCount() > iCount)
                    {
                        ACE_OS::sleep(KtvInter);
                    }
                }
            }
            m_mapCmdHandlers.clear();
        }

        {
            ACE_GUARD(ACE_Thread_Mutex, localmx, m_mxCmdEx);
            m_mapCmdHandlerExs.clear();
        }

        m_pClientManage->CancelAllCallback();
    }

    void CommunicationProxy::Implement::UnRegisterDataHandler( void )
    {
        UnRegisterCommandHandler(SYSTEM_COMMAND_ID_SHAREMEMORY_BIGDATA_TRANSFER);    
    }

    void CommunicationProxy::Implement::UnRegisterEventHandler( int iChannelId, int iEventId )
    {
        ACE_GUARD(ACE_Thread_Mutex, localmx, m_mxEvent);

        EventKey mapkey(iChannelId, iEventId);
        int iCount = 1;
        multimapRange range = m_mapEventHandlers.equal_range(mapkey);
        std::map<boost::thread::id, EventKey>::const_iterator it = m_mapThdId2EvtKey.find(boost::this_thread::get_id());
        if (it != m_mapThdId2EvtKey.end() &&
            it->second.first == iChannelId && it->second.second == iEventId)
        {
           iCount = 2;
        }
        for(EventId2Handler::const_iterator itr = range.first; itr != range.second; ++itr )
        {
            while(itr->second->GetCount() > iCount)
            {
                ACE_OS::sleep(KtvInter);
            }
        }

        m_mapEventHandlers.erase(mapkey);
    }

    void CommunicationProxy::Implement::UnRegisterAllEventHandlers()
    {
        NTB_TRACE_INFO_WITH_THIS("UnRegisterAllEventHandlers enter.");
        {
            ACE_GUARD(ACE_Thread_Mutex, localMutex, m_mxEvent);
            EventId2Handler::iterator it = m_mapEventHandlers.begin();
            std::map<boost::thread::id, EventKey>::const_iterator itTid2Evtkey = m_mapThdId2EvtKey.find(boost::this_thread::get_id());
            for (; it != m_mapEventHandlers.end(); ++it)
            {
                int iCount = 1;
                entHandlerPtr tempPtr = it->second;
                if (tempPtr)
                {
                    if (itTid2Evtkey != m_mapThdId2EvtKey.end() && it->first == itTid2Evtkey->second)
                    {
                        iCount = 2;
                    }
                    while(tempPtr->GetCount() > iCount)
                    {
                        ACE_OS::sleep(KtvInter);
                    }
                }
            }
            m_mapEventHandlers.clear();
        }

        m_pClientManage->CancelAllCallback();
    }

    void CommunicationProxy::Implement::UnRegisterAllHandlers()
    {
        UnRegisterAllCommandHandlers();
        UnRegisterAllEventHandlers();
        UnRegisterDataHandler();
    }

    bool CommunicationProxy::Implement::IsCommand( int id )
    {
        NTB_TRACE_INFO_WITH_THIS("IsCommand cmd_id="<<id);
        ACE_GUARD_RETURN(ACE_Thread_Mutex, localmx, m_mxCmd, false);
        return m_mapCmdHandlers.find(id) != m_mapCmdHandlers.end();
    }

    bool CommunicationProxy::Implement::IsCommandEx( int id )
    {
        ACE_GUARD_RETURN(ACE_Thread_Mutex, localmx, m_mxCmdEx, false);
        return m_mapCmdHandlerExs.find(id) != m_mapCmdHandlerExs.end();
    }

    boost::shared_ptr<ICommandHandlerEx>
    CommunicationProxy::Implement::GetCmdExHandler( int id )
    {
        NTB_TRACE_INFO_WITH_THIS("get command handlerex by command id.cmd_id="<<id);
        boost::shared_ptr<ICommandHandlerEx> nullPtr;
        ACE_GUARD_RETURN(ACE_Thread_Mutex, localmx, m_mxCmdEx, nullPtr);
        CmdId2HandlerEx::iterator it = m_mapCmdHandlerExs.find(id);
        if (it == m_mapCmdHandlerExs.end())
        {
            NTB_TRACE_INFO_WITH_THIS("can't find command handler by command id.cmd_id="<<id);
            return nullPtr;
        }
        return it->second;
    }

    cmdHandlerPtr CommunicationProxy::Implement::GetCmdHandler( int id )
    {
        NTB_TRACE_INFO_WITH_THIS("get command handler by command id.cmd_id="<<id);
        cmdHandlerPtr nullPtr;
        ACE_GUARD_RETURN(ACE_Thread_Mutex, localmx, m_mxCmd, nullPtr);
        CmdId2Handler::iterator it = m_mapCmdHandlers.find(id);
        if (it == m_mapCmdHandlers.end())
        {
            NTB_TRACE_INFO_WITH_THIS("can't find command handler by command id.cmd_id="<<id);
            return nullPtr;
        }
        it->second->Duplicate();
        return it->second;
    }

    int CommunicationProxy::Implement::CalcDataSenderAddr( const std::string &sReceiver, 
        IDataSender * pSender, std::string &sSendPeerPort, std::string &sSendPeerIP )
    {
        static const std::string sLocalHost = "127.0.0.1";
        int iRtn = CheckCastToName(sReceiver);
        if (iRtn != McsfSuccess)
        {
            NTB_LOG_ERROR_WITH_THIS("send data checkcast name failed.iRtn="<<iRtn);
            return iRtn;
        }

        std::string sPeerAddr = m_pClientManage->GetReceiverIp(sReceiver);
        if (sPeerAddr.empty())
        {
            NTB_LOG_ERROR_WITH_THIS("GetReceiverIp is empty!");
            return McsfUndefineError;
        }

        std::vector<std::string> vPeerAddr;
        boost::algorithm::split(vPeerAddr, sPeerAddr, boost::is_any_of("&:"));

        std::vector<std::string> vSendAddr;
        std::string sTemp = pSender->GetName();
        NTB_TRACE_INFO_WITH_THIS("All sender addr="<<sTemp);

        boost::algorithm::split(vSendAddr, sTemp, boost::is_any_of("&:"));
        sSendPeerPort = vSendAddr.back();
        vSendAddr.pop_back();
        if (!vPeerAddr.at(0).compare(sLocalHost))
        {
            sSendPeerIP = sLocalHost;
            return iRtn;
        }

        if (!vPeerAddr.empty())
        {
            sSendPeerIP = FindSimilarIPAddr(vSendAddr, vPeerAddr.at(0));
        }

        return iRtn;
    }

    //////////////////////////////////////////////////////////////////////////

    CommunicationProxy::CommunicationProxy( int iReactorPriority
        ) : m_pImpl(new CommunicationProxy::Implement(iReactorPriority))
    {
    }

    CommunicationProxy::~CommunicationProxy()
    {
        DEL_PTR( m_pImpl );
    }

    int CommunicationProxy::RegisterCommandHandler( int iCommandId, 
        ICommandHandler* pCommandHandler )
    {
        return m_pImpl->RegisterCommandHandler(iCommandId, pCommandHandler);
    }

    int CommunicationProxy::RegisterCommandHandlerEx( int iCommandId, 
        boost::shared_ptr<ICommandHandlerEx> pCommandHandler )
    {
        return m_pImpl->RegisterCommandHandlerEx(iCommandId, pCommandHandler);
    }

    int CommunicationProxy::RegisterEventHandler( int iChannelId, int iEventId, 
        IEventHandler* pEventHandler )
    {
        return m_pImpl->RegisterEventHandler( iChannelId, iEventId, pEventHandler );
    }

    int CommunicationProxy::SyncSendData( const SendDataContext* pSendDataContext )
    {
        return m_pImpl->SyncSendData( pSendDataContext );
    }

    int CommunicationProxy::SyncSendDataEx( const SendDataContextEx& pSendDataContext, 
        std::string& sSerializeObject )
    {
        return m_pImpl->SyncSendDataEx( &pSendDataContext, sSerializeObject );
    }

    ISyncResult* CommunicationProxy::SyncSendDataEx( const SendDataContextEx& pSendContext )
    {
        return m_pImpl->SyncSendDataEx(&pSendContext);
    }

    int CommunicationProxy::AsyncSendDataEx( const SendDataContextEx& pSendDataContext )
    {
        return m_pImpl->AsyncSendDataEx( &pSendDataContext );
    }

    int CommunicationProxy::AsyncSendData( const SendDataContext* pSendDataContext )
    {
        return m_pImpl->AsyncSendData( pSendDataContext );
    }

    int CommunicationProxy::SyncSendCommand( const CommandContext* pCommandContext, 
        std::string& sSerializeObject )
    {
        return m_pImpl->SyncSendCommand( pCommandContext, sSerializeObject );
    }

    ISyncResult* CommunicationProxy::SyncSendCommand( const CommandContext* pCommandContext )
    {
        return m_pImpl->SyncSendCommand( pCommandContext );
    }

    int CommunicationProxy::AsyncSendCommand( const CommandContext* pCommandContext )
    {
        return m_pImpl->AsyncSendCommand( pCommandContext );
    }

    int CommunicationProxy::SendEvent( const std::string& sSender, int iEventId,
        const std::string& sEvent )
    {
        return m_pImpl->SendEvent( sSender, iEventId, sEvent );
    }

    int CommunicationProxy::SendEvent( const std::string& sSender, int iEventId, std::string& sEvent )
    {
        return m_pImpl->SendEvent( sSender, iEventId, sEvent );
    }

    int CommunicationProxy::DispatchEvent( const std::string& sReceiver,
        const std::string& sSender, int iChannelId,
        int iEventId, const std::string& sEvent )
    {
        return m_pImpl->DispatchEvent( sReceiver, 
            sSender, iChannelId, iEventId, sEvent, false );
    }
    int CommunicationProxy::DispatchEvent( const std::string &sIpAddr, const std::string& sReceiver,
        const std::string& sSender,
        int iChannelId, 
        int iEventId, const std::string& sEvent )
    {
        return m_pImpl->DispatchEvent( sIpAddr, sReceiver, 
            sSender, iChannelId, iEventId, sEvent, false );
    }

    int CommunicationProxy::BroadcastEvent( const std::string& sSender, int iEventId, 
        const std::string& sEvent )
    {
        return m_pImpl->BroadcastEvent( sSender, iEventId, sEvent );
    }

    int CommunicationProxy::StartListener( const std::string& sIpAddr )
    {
        return m_pImpl->StartListener(sIpAddr);
    }

    int CommunicationProxy::CheckCastToSystemDispatcher( const std::string& sDispatchIpAddr )
    {
        return m_pImpl->CheckCastToSystemDispatcher( sDispatchIpAddr );
    }

    int CommunicationProxy::SubscribeEvent( const ChannelIdMap& idMap )
    {
        return m_pImpl->SubscribeEvent( idMap );
    }

    int CommunicationProxy::SubscribeEvent( int iChannelId, int iEventId )
    {
        return m_pImpl->SubscribeEvent( iChannelId, iEventId );
    }

    int CommunicationProxy::RegisterDataHandler( IDataHandler* pSendDataHandler )
    {
        return m_pImpl->RegisterDataHandler(pSendDataHandler);
    }

    int CommunicationProxy::RegisterDataHandlerEx( boost::shared_ptr<IDataHandlerEx> pSendDataHandler )
    {
        return m_pImpl->RegisterDataHandlerEx(pSendDataHandler);
    }

    void CommunicationProxy::SetName( const std::string& sName )
    {
        m_pImpl->SetName( sName );
    }

    std::string CommunicationProxy::GetName() const
    {
        return m_pImpl->GetName(); 
    }

    void CommunicationProxy::SetListenAddress( const std::string& sIPAdress )
    {
        return m_pImpl->SetListenAddress(sIPAdress);
    }

    std::string CommunicationProxy::GetListenAddress() const
    {
        return m_pImpl->GetListenAddress(); 
    }

    std::string CommunicationProxy::GetMacAddress() const
    {
        return m_pImpl->GetMacAddress();
    }

    void CommunicationProxy::SetSenderChannelId( int iChannelId )
    {
        m_pImpl->SetSenderChannelId( iChannelId );
    }

    int CommunicationProxy::GetSenderChannelId( void ) const
    {
        return m_pImpl->GetSenderChannelId();
    }

    int CommunicationProxy::CheckCastToRemote( const std::string& sName,
        const std::string& sIpAddr )
    {
        return m_pImpl->CheckCastToRemote(sName, sIpAddr);
    }

    std::string CommunicationProxy::GetSystemDispatcherAddress( void ) const
    {
        return m_pImpl->GetSystemDispatcherAddress();
    }

    int CommunicationProxy::InitNetBase( int argc, char* argv[] )
    {
        return m_pImpl->InitNetBase(argc, argv);
    }

    int CommunicationProxy::SendSystemEvent( const std::string& sSender, int iEventId, const std::string& sEvent )
    {
        return m_pImpl->SendSystemEvent( sSender, iEventId, sEvent );
    }

    int CommunicationProxy::BroadcastSystemEvent( const std::string& sSender, int iEventId, const std::string& sEvent )
    {
        return m_pImpl->BroadcastSystemEvent( sSender, iEventId, sEvent );
    }

    int CommunicationProxy::CheckCastName( const std::string& sName, int iWaitTime )
    {
        return m_pImpl->CheckCastToName( sName, iWaitTime );
    }

    void CommunicationProxy::UnRegisterCommandHandler( int iCommandId )
    {
        return m_pImpl->UnRegisterCommandHandler(iCommandId);
    }

    void CommunicationProxy::UnRegisterAllCommandHandlers( void )
    {
        return m_pImpl->UnRegisterAllCommandHandlers();
    }

    void CommunicationProxy::UnRegisterDataHandler( void )
    {
        return m_pImpl->UnRegisterDataHandler();
    }

    void CommunicationProxy::UnRegisterEventHandler( int iChannelId, int iEventId )
    {
        return m_pImpl->UnRegisterEventHandler(iChannelId, iEventId);
    }

    void CommunicationProxy::UnRegisterAllEventHandlers()
    {
        return m_pImpl->UnRegisterAllEventHandlers();
    }

    void CommunicationProxy::UnRegisterAllHandlers()
    {
        return m_pImpl->UnRegisterAllHandlers();
    }

    void CommunicationProxy::RegisterCallBackFunction(EVENTTYPE eEvtType, boost::function1<void,void*> pFunction)
    {
        m_pImpl->RegisterCallBackFunction(eEvtType, pFunction);
    }

    void CommunicationProxy::UnregisterCallBackFunction(EVENTTYPE eEvtType)
    {
        m_pImpl->UnregisterCallBackFunction(eEvtType);
    }
}

