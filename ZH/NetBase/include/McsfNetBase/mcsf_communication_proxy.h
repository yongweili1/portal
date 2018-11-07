////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_communication_proxy.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/01
///
/// \brief  the main export class by net base library to do this feature
///         Synchronous Command Call
///         Asynchronous Command Call
///         Send Event ( Command not request response )
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_COMMUNICATIONPROXY_H_
#define _MCSF_COMMUNICATIONPROXY_H_

#include <vector>
#include <map>
#include <set>

#include "mcsf_netbase_export.h"
#include "mcsf_netbase_interface.h"
#include "boost/function.hpp"
// if you compile with swig.exe, you have to do this
#ifdef SWIG
#define McsfNetBase_Export
#endif

#ifdef _MSC_VER
#pragma warning(disable:4251)
#endif // _MSC_VER

namespace Mcsf {

    //class UnregisterCommProxyFunc;
    /// \class CommunicationProxy
    /// \brief An communication proxy that as server to receive command call
    /// and as client to send command call.
    /// here is an example
    /// \verbatim
    /// CommunicationProxy m_pComm = new CommunicationProxy();
    /// m_pComm->SetContainerName( "BlobTest" );
    /// std::string sSysDispatcherAddr = "127.0.0.1:10000";
    /// m_pComm->CheckCastToSystemDispatcher( sSysDispatcherAddr );
    /// // if you want to receive an command call and event you also need do
    /// m_pComm->StartListener( "127.0.0.1" );
    /// // than you can do as client to 
    /// m_pComm->SendEvent("", 1, "hello world");
    /// m_pComm->SyncSendCommand( &context, sResponse ); 
    /// // also you can as server to handle command call or event
    /// class EchoTestCmdHandler : public ICommandHandler {}
    /// m_pComm->RegisterCommandHandler( CONTAINER_TEST_ECHO_CMD_ID, 
    ///        *         new EchoTestCmdHandler() );
    /// // when complete you can do like this to destroy server and client resource
    /// m_pSysDispatcherComm->ShutDown();
    /// \endverbatim
    class McsfNetBase_Export CommunicationProxy : public ICommunicationProxy
    {
    public:
        typedef std::set<int> IdSet;
        typedef std::map<int, IdSet> ChannelIdMap;

    public:
        /// Constructor.
        /// All netbase socket events are registered into a thread, this thread be called reactor in ACE;
        /// to improve socket data received or connection disconnected timely, you can create it with high priority reactor.
        /// [windows] support these parameters, default is THREAD_PRIORITY_NORMAL =0 ;
        ///             THREAD_PRIORITY_IDLE
        ///             THREAD_PRIORITY_LOWEST
        ///             THREAD_PRIORITY_BELOW_NORMAL
        ///             THREAD_PRIORITY_NORMAL
        ///             THREAD_PRIORITY_ABOVE_NORMAL
        ///             THREAD_PRIORITY_HIGHEST         (suggest this one, if timeout happens frequently)
        ///             THREAD_PRIORITY_TIME_CRITICAL
        ///
        /// [linux]  if set priority is not zero(1~99), the reactor thread will use SCHED_RR policy to run.
        explicit CommunicationProxy(int iReactorPriority = 0);
        /// Destructor.
        virtual ~CommunicationProxy();

        /// Send an command call asynchronously, when an command response receive
        /// the callback in \a pCommandContext will be called.
        /// here is an example
        /// \verbatim
        /// class LongCmdCallBack : public ICommandCallbackHandler {}
        ///
        /// McsfPerformance request;      // google protocol buffer generated class
        /// request.set_id( 123 );
        /// request.set_is_male( false );
        /// CommandContext context;
        /// context.sReceiver = "SystemDispatcher";
        /// context.iCommandId = CONTAINER_TEST_ECHO_CMD_ID;
        /// context.sSerializeObject = request.SerializeAsString();
        /// context.pCommandCallback = new LongCmdCallBack();
        ///
        /// m_pComm->AsyncSendCommand( &context );
        /// \endverbatim
        /// \param   pCommandContext    command context     
        /// \return  int        *       return 0 if success
        virtual int AsyncSendCommand(const CommandContext* pCommandContext);

        /// send an command call synchronously, the call will block until response 
        /// return 
        /// here is an example
        /// \verbatim
        /// McsfPerformance request;      // google protocol buffer generated class
        /// request.set_id( 123 );
        /// request.set_is_male( false );
        /// CommandContext context;
        /// context.sReceiver = "SystemDispatcher";
        /// context.iCommandId = CONTAINER_TEST_ECHO_CMD_ID;
        /// context.sSerializeObject = request.SerializeAsString();
        /// std::string sResponse("");
        /// m_pComm->SyncSendCommand( &context, sResponse );
        /// \endverbatim
        /// \param   pCommandContext      command context    
        /// \param   sSerializeObject     return 0 if success    
        /// \return  int 
        virtual int SyncSendCommand( const CommandContext* pCommandContext, 
            std::string& sSerializeObject );

        /// send an command call synchronously, the call will block until response 
        /// return . Add this overload function just make it easy for SWIG generate
        /// C# Code.
        /// here is an example
        /// \verbatim
        /// McsfPerformance request;      // google protocol buffer generated class
        /// request.set_id( 123 );
        /// request.set_is_male( false );
        /// CommandContext context;
        /// context.sReceiver = "SystemDispatcher";
        /// context.iCommandId = CONTAINER_TEST_ECHO_CMD_ID;
        /// context.sSerializeObject = request.SerializeAsString();
        /// ISyncResult* pResult = m_pComm->SyncSendCommand( &context );
        /// 
        /// std::string sResult = pResult->GetMarshalObject();
        /// \endverbatim
        /// note i am sorry guy, you have pay attention to delete the pResult
        /// \param   pCommandContext     command context    
        /// \return  ISyncResult*        
        virtual ISyncResult* SyncSendCommand( const CommandContext* pCommandContext);

        /// Send raw blob data. optimize for big data. if you send data in same machine 
        /// with use share memory as IPC.
        /// 
        /// \param   pSendDataContext         
        /// \return  int 
        virtual int SyncSendData(const SendDataContext* pSendDataContext);
        virtual int SyncSendDataEx(const SendDataContextEx& pSendContext, 
            std::string& sSerializeObject);
        /// only for SWIG generated code, don't use this in C++ Code 
        virtual ISyncResult* SyncSendDataEx(const SendDataContextEx& pSendContext);
        /// asynchronous send raw data. In same PC use share memory, and cross PC use 
        /// socket 
        /// 
        /// \param   pSendDataContext   the context of send data      
        /// \return  int        *       return 0 if success.
        virtual int AsyncSendData( const SendDataContext* pSendDataContext );
        virtual int AsyncSendDataEx(const SendDataContextEx& pSendContext);

        /// register command handler associate with \a iCommandId. it is your responsibility
        /// to manage \a pCommandHandler life time. 
        /// 
        /// \param   iCommandId        *     the unique command id
        /// \param   pCommandHandler         the handler class
        /// \return  int 
        virtual int RegisterCommandHandler(int iCommandId, 
            ICommandHandler* pCommandHandler);
        virtual int RegisterCommandHandlerEx(int iCommandId, 
            boost::shared_ptr<ICommandHandlerEx> pCommandHandler);

        /// unregister command handler in service side
        /// 
        /// \param   int iCommandId  the unique command id       
        /// \return  int return zero 
        virtual void UnRegisterCommandHandler(int iCommandId);
        /// unregister All command handler in service side
        virtual void UnRegisterAllCommandHandlers(void);

        /// register raw blob data handler call back
        virtual int RegisterDataHandler(IDataHandler* pSendDataHandler);
        virtual int RegisterDataHandlerEx(boost::shared_ptr<IDataHandlerEx> pSendDataHandler);

        /// unregister big data hander class
        virtual void UnRegisterDataHandler(void);

        /// register event handler associate with channel id and event id. it's your
        /// responsibility to manage \a pEventHandler life time
        /// 
        /// \param   iChannelId       the containee channel id 
        /// \param   iEventId         the event id
        /// \param   pEventHandler    the handler class 
        /// \return  int 
        virtual int RegisterEventHandler( int iChannelId, int iEventId, 
            IEventHandler* pEventHandler);

        /// register an event handler to communication proxy 
        /// 
        /// \param   iChannelId       the event sender channel id     
        /// \param   iEventId         the event id
        virtual void UnRegisterEventHandler(int iChannelId, int iEventId) ;
        /// unregister all event handler to communication proxy 
        virtual void UnRegisterAllEventHandlers() ;

        /// unregister all handler, including command , event and big data handler
        virtual void UnRegisterAllHandlers();

        /// Send event to \a sSender
        virtual int SendEvent( const std::string& sSender, int iEventId, 
            const std::string& sEvent );
// 
//         /// send event to \a sSender, just for SWIG generate code, useless in native C++
//         virtual int SendEvent(const std::string& sSender, int iEventId,
//             std::string& sEvent );

        /// not implement
        virtual int BroadcastEvent( const std::string& sSender, int iEventId, 
            const std::string& sEvent );

        /// subscribe event to SysDispatcher
        /// 
        /// \param   idMap      the channel and event id that subscribe
        /// \return  int 
        virtual int SubscribeEvent( const ChannelIdMap& idMap );

        /// subscribe event to SysDispatcher
        /// 
        /// \param   iChannelId     the event channel id    
        /// \param   iEventId       the event id
        /// \return  int 
        virtual int SubscribeEvent( int iChannelId, int iEventId );

        /// start an server at \a sIpAddr and register it to SysDispatcher
        /// 
        /// \param   sIpAddr       the ip address which can be"ip-number:port-number" 
        ///        *              (e.g., "tango.cs.wustl.edu:1234" or  "128.252.166.57:1234").
        ///        *              If there is no ':' in the \a address it* is assumed to be a
        ///        *              ip number. And auto assign an listen number   
        /// \return  int 
        int StartListener( const std::string& sIpAddr );

        int CheckCastToSystemDispatcher( const std::string& sDispatchIpAddr );
        virtual int CheckCastName(const std::string& sName, int iWaitTime);

        /// set container name
        void SetName( const std::string& sName ); 

        /// get container name
        virtual std::string GetName() const;

        /// set container channel id
        void SetSenderChannelId(int iChannelId);

        /// get container channel id
        int GetSenderChannelId( void ) const;

        /// used for event forwarding by system dispatcher
        virtual int DispatchEvent( const std::string& sReceiver,
            const std::string& sSender,
            int iChannelId, 
            int iEventId, const std::string& sEvent );

        virtual int DispatchEvent( const std::string &sIpAddr, const std::string& sReceiver,
            const std::string& sSender,
            int iChannelId, 
            int iEventId, const std::string& sEvent );


        /// used only for system dispatcher 
        virtual int CheckCastToRemote( const std::string& sName,
            const std::string& sIpAddr );

        void SetListenAddress( const std::string& sIPAdress );
        std::string GetListenAddress() const;
        std::string GetMacAddress() const;

        virtual std::string GetSystemDispatcherAddress(void) const;

        int InitNetBase(int argc, char* argv[]);

        virtual int SendSystemEvent( const std::string& sSender, 
            int iEventId, const std::string& sEvent ) ;

        virtual int BroadcastSystemEvent( const std::string& sSender,
            int iEventId, const std::string& sEvent ) ;
#ifndef SWIG
        /// register callback function, only used by SystemDispatcher
        virtual void RegisterCallBackFunction(EVENTTYPE eEvtType, boost::function1<void,void*> pFunction);
        /// unregister caback function
        virtual void UnregisterCallBackFunction(EVENTTYPE eEvtType);
#endif

    private:
        class Implement;
        Implement* m_pImpl;
        int m_iPriority;

        int m_iChannelId;
        std::string m_sDispatcherAddr;
        std::string m_sName;
        std::string m_sListenPort;
    };

}

#endif
