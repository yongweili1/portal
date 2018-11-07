#ifndef COMMUNICATIONPROXYMOCK_H_
#define COMMUNICATIONPROXYMOCK_H_

#include "mcsf_netbase_icommunication_proxy.h"

namespace Mcsf {
    class CommunicationProxyMockSuccessful : public ICommunicationProxy
    {
        virtual int AsyncSendCommand(const CommandContext* ) {return 0;};
        virtual int SyncSendData(const SendDataContext* ) {return 0;};
        virtual int SyncSendDataEx(const SendDataContextEx& , std::string& ) {return 0;};
        virtual int AsyncSendData( const SendDataContext* ) {return 0;}; 
        virtual int AsyncSendDataEx(const SendDataContextEx&) {return 0;};
        virtual int RegisterCommandHandler(int , ICommandHandler* ) {return 0;};
        virtual int RegisterCommandHandlerEx(int,boost::shared_ptr<ICommandHandlerEx> ) {return 0;};
        virtual int RegisterDataHandler(IDataHandler*) {return 0;};
        virtual int RegisterDataHandlerEx(boost::shared_ptr<IDataHandlerEx> ) {return 0;};
        virtual int SendEvent( const std::string& , int , const std::string&  ) {return 0;};
        virtual int BroadcastEvent( const std::string&, int , const std::string&  ) {return 0;};
        virtual int RegisterEventHandler( int /*iChannelId*/, 
            int /*iEventId*/, IEventHandler* /*pEventHandler*/) {return 0;};
        virtual int SyncSendCommand( const CommandContext* /*pCommandContext*/, 
            std::string& /*sSerializeObject*/ ) {return 0;};
        virtual int CheckCastName(const std::string& /*sName*/, int /*iWaitTime*/) {return 0;};
        virtual std::string GetName() const {return "";};
        virtual std::string GetSystemDispatcherAddress(void) const {return "";};
        virtual int SendSystemEvent( const std::string& /*sSender*/, 
            int /*iEventId*/, const std::string& /*sEvent*/ ) {return 0;};
        virtual int BroadcastSystemEvent( const std::string& /*sSender*/,
            int /*iEventId*/, const std::string& /*sEvent*/ ) {return 0;};

		virtual void UnRegisterCommandHandler(int ) {};
		/// unregister All command handler in service side
		virtual void UnRegisterAllCommandHandlers(void) {};

		virtual void UnRegisterDataHandler(void) {};

		virtual void UnRegisterEventHandler(int , int ) {};
		/// unregister all event handler to communication proxy 
		virtual void UnRegisterAllEventHandlers() {};

		/// unregister all handler, including command , event and big data handler
		virtual void UnRegisterAllHandlers() {};
    };

    class CommunicationProxyMockFailed : public ICommunicationProxy
    {
        virtual int AsyncSendCommand(const CommandContext* ) {return -1;};
        virtual int SyncSendData(const SendDataContext* ) {return -1;};
        virtual int SyncSendDataEx(const SendDataContextEx& , std::string& ) {return -1;};
        virtual int AsyncSendData( const SendDataContext* ) {return -1;}; 
        virtual int AsyncSendDataEx(const SendDataContextEx&) {return -1;};
        virtual int RegisterCommandHandler(int , ICommandHandler* ) {return -1;};
        virtual int RegisterCommandHandlerEx(int,boost::shared_ptr<ICommandHandlerEx> ) {return -1;};
        virtual int RegisterDataHandler(IDataHandler*) {return -1;};
        virtual int RegisterDataHandlerEx(boost::shared_ptr<IDataHandlerEx> ) {return -1;};
        virtual int SendEvent( const std::string& , int , const std::string&  ) {return -1;};
        virtual int BroadcastEvent( const std::string&, int , const std::string&  ) {return -1;};
        virtual int RegisterEventHandler( int /*iChannelId*/, 
            int /*iEventId*/, IEventHandler* /*pEventHandler*/) {return -1;};
        virtual int SyncSendCommand( const CommandContext* /*pCommandContext*/, 
            std::string& /*sSerializeObject*/ ) {return -1;};
        virtual int CheckCastName(const std::string& /*sName*/, int /*iWaitTime*/) {return -1;};
        virtual std::string GetName() const {return "";};
        virtual std::string GetSystemDispatcherAddress(void) const {return "";};
        virtual int SendSystemEvent( const std::string& /*sSender*/, 
            int /*iEventId*/, const std::string& /*sEvent*/ ) {return -1;};
        virtual int BroadcastSystemEvent( const std::string& /*sSender*/,
            int /*iEventId*/, const std::string& /*sEvent*/ ) {return -1;};

		virtual void UnRegisterCommandHandler(int ) {};
		/// unregister All command handler in service side
		virtual void UnRegisterAllCommandHandlers(void) {};

		virtual void UnRegisterDataHandler(void) {};

		virtual void UnRegisterEventHandler(int , int ) {};
		/// unregister all event handler to communication proxy 
		virtual void UnRegisterAllEventHandlers() {};

		/// unregister all handler, including command , event and big data handler
		virtual void UnRegisterAllHandlers() {};
    };
}

#endif // COMMUNICATIONPROXYMOCK_H_
