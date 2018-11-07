////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_netbase_icommunication_proxy.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/07
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_NETBASE_ICOMMUNICATION_PROXY_H_
#define MCSF_NETBASE_ICOMMUNICATION_PROXY_H_

#include <string>
#include <boost/shared_ptr.hpp>

#include "mcsf_netbase_export.h"

// if you compile with swig.exe, you have to do this
#ifdef SWIG
#define McsfNetBase_Export
#endif

namespace Mcsf {
    struct CommandContext;
    struct SendDataContext;

    class ICommandCallbackHandler;
    class IAsyncResult;
    class ICommandHandler;
    class ISyncResult;
    class ICommunicationProxy;
    class ISendDataCallbackHandler;
    class IDataHandler;
    class IEventHandler;

    class ICommandHandlerEx;
    struct SendDataContextEx;
    class IDataHandlerEx;
}

namespace Mcsf {

    /// \class ICommunicationProxy
    /// \brief the interface used by containee to sync/async send command or send event.
    /// or process command request or event. user can just use McsfNetBase Implement 
    /// CommunicationProxy. see mcsf_communication_proxy.h
    /// here is an example
    /// \verbatim
    /// class myContainee : public IContaineeBase
    /// {
    /// public:
    ///   virtual void Startup() 
    ///   {
    ///      //m_pComm will be register by IContainee function SetContainer.
    ///      //consumer can user McsfNetBase export class CommunicationProxy
    ///      m_pComm->RegisterCommandHandler( 1, new CommandHandlerOne() );
    ///   }
    ///   
    ///   void Dowork()
    ///   {
    ///      m_pComm->AsyncSendCommand();
    ///     m_pComm->SyncSendCommand(); etc.
    ///   }
    /// }
    /// \endverbatim
    /// 
    class McsfNetBase_Export ICommunicationProxy
    {
    public:
        virtual ~ICommunicationProxy() { }

        /// asynchronous command send, make sure pCommandCallback in CommandContext
        /// is not null. the underly service thread will callback that.
        ///<DSKey>
        ///ID:117582
        ///Title:DS_PRA_NetBase_DataTransmissionFailure
        ///</DSKey>
        virtual int AsyncSendCommand(const CommandContext* pCommandContext) = 0;

        /// send blob data synchronous, block until data send complete
        /// [deprecated, please use SyncSendDataEx for more power memory management
        virtual int SyncSendData(const SendDataContext* pSendDataContext) = 0;

        ///<DSKey>
        ///ID:117582
        ///Title:DS_PRA_NetBase_DataTransmissionFailure
        ///</DSKey>
        virtual int SyncSendDataEx(const SendDataContextEx& pSendContext,
            std::string& sSerializeObject) = 0;

        /// [deprecated]use SyncSendDataEx for more power memory management
        virtual int AsyncSendData(const SendDataContext* pSendDataContext) = 0;

        ///<DSKey>
        ///ID:117582
        ///Title:DS_PRA_NetBase_DataTransmissionFailure
        ///</DSKey>
        virtual int AsyncSendDataEx(const SendDataContextEx& pSendContext) = 0;

        /// register command handler in service side
        /// 
        /// \param   int iCommandId  the unique command id       
        /// \param   ICommandHandler * pCommandHandler command handler inherit ICommandHandler 
        ///          note: the communication proxy don't maintain life cycle of command handler       
        virtual int RegisterCommandHandler(int iCommandId, ICommandHandler* pCommandHandler) = 0;

        /// register command handler in service side which don't send response immediately
        virtual int RegisterCommandHandlerEx(int iCommandId, boost::shared_ptr<ICommandHandlerEx> pCommandHandler) = 0;

        /// unregister command handler in service side
        /// 
        /// \param   int iCommandId  the unique command id       
        /// \return 
        ///    McsfUndefineError = -1,        
        ///    McsfSuccess = 0,        
        virtual void UnRegisterCommandHandler(int iCommandId) = 0;

        /// unregister All command handler in service side
        virtual void UnRegisterAllCommandHandlers(void) = 0;


        /// register big data hander class
        /// [deprecated, please use SyncSendDataEx for more power memory management
        virtual int RegisterDataHandler(IDataHandler* pSendDataHandler) = 0;


        virtual int RegisterDataHandlerEx(boost::shared_ptr<IDataHandlerEx> pSendDataHandler) = 0;

        /// unregister big data hander class    
        /// \return 
        ///    McsfUndefineError = -1,        
        ///    McsfSuccess = 0,        
        virtual void UnRegisterDataHandler(void) = 0;

        /// send event to who registered @iEventId 
        ///<DSKey>
        ///ID:117582
        ///Title:DS_PRA_NetBase_DataTransmissionFailure
        ///</DSKey>
        virtual int SendEvent(const std::string& sSender, int iEventId, const std::string& sEvent) = 0;

        /// send event to all onlines.
        ///<DSKey>
        ///ID:117582
        ///Title:DS_PRA_NetBase_DataTransmissionFailure
        ///</DSKey>
        virtual int BroadcastEvent(const std::string& sSender, int iEventId, const std::string& sEvent) = 0;

        /// register an event handler to communication proxy 
        /// 
        /// \param   iChannelId       the event sender channel id  [deprecated]   
        /// \param   iEventId         the event id
        /// \param   pEventHandler    the event handler class (note: the communication
        ///                           don't maintain the life cycle of event handler)     
        virtual int RegisterEventHandler(int iChannelId, int iEventId, IEventHandler* pEventHandler) = 0;

        /// register an event handler to communication proxy 
        /// \return 
        ///    McsfUndefineError = -1,        
        ///    McsfSuccess = 0,        
        /// \param   iChannelId       the event sender channel id   [deprecated]
        /// \param   iEventId         the event id
        virtual void UnRegisterEventHandler(int iChannelId, int iEventId) = 0;

        /// unregister all event handler to communication proxy         
        /// \return 
        ///    McsfUndefineError = -1,        
        ///    McsfSuccess = 0,            
        virtual void UnRegisterAllEventHandlers() = 0;

        /// unregister all handler, including command , event and big data handler

        /// \return 
        ///    McsfUndefineError = -1,        
        ///    McsfSuccess = 0,           
        virtual void UnRegisterAllHandlers() = 0;

        /// synchronous command send, the thread will block until command return.
        /// 
        /// \param   const CommandContext * pCommandContext         
        /// \param   std::string & sSerializeObject return result serialized in string.       
        ///<DSKey>
        ///ID:117582
        ///Title:DS_PRA_NetBase_DataTransmissionFailure
        ///</DSKey>
        virtual int SyncSendCommand(const CommandContext* pCommandContext, std::string& sSerializeObject) = 0;

        /// try to connect to communication node till time out
        /// \return  number in mcsf_error_code.h
        /// \param   sName         Communication Node Name
        /// \param   iWaitTime     wait timeout 
        /// \return  int 
        virtual int CheckCastName(const std::string& sName, int iWaitTime) = 0;

        ///
        /// \brief Retrieve Communication Proxy name, which is also called Containee Name.
        ///
        /// \return
        virtual std::string GetName() const = 0;

        /// \brief get the underly system dispatcher IP address
        /// 
        /// \param        *   
        /// \return  std::string 
        virtual std::string GetSystemDispatcherAddress(void) const = 0;

        ///  \brief      send system event with channel 0, this event will be
        ///              cared by anyone who subscribed
        /// 
        ///  \param[in]  sSender if empty will fill with communication name
        ///                      else keep not change
        ///  \param[in]  iEventId System ID
        ///  \param[in]  sEvent what need to send       
        ///<DSKey>
        ///ID:117582
        ///Title:DS_PRA_NetBase_DataTransmissionFailure
        ///</DSKey>
        virtual int SendSystemEvent(const std::string& sSender, int iEventId, const std::string& sEvent) = 0;

        ///  \brief      broadcast system event with channel 0, this event will be
        ///              cared by anyone who subscribed
        /// 
        ///  \param[in]  sSender whatever,please omit it
        ///  \param[in]  iEventId System ID
        ///  \param[in]  sEvent what need to send
        ///<DSKey>
        ///ID:117582
        ///Title:DS_PRA_NetBase_DataTransmissionFailure
        ///</DSKey>
        virtual int BroadcastSystemEvent(const std::string& sSender, int iEventId, const std::string& sEvent) = 0;
    };
}

#endif // MCSF_NETBASE_ICOMMUNICATION_PROXY_H_
