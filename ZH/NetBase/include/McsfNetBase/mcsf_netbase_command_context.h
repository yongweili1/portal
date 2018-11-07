////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_netbase_command_context.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/07
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_NETBASE_COMMAND_CONTEXT_H_
#define MCSF_NETBASE_COMMAND_CONTEXT_H_

#include <string>

#include "mcsf_netbase_export.h"

namespace Mcsf 
{
    class ICommandCallbackHandler;
     
    /// \class CommandContext
    /// \brief synchronous and asynchronous command call parameter, you can send command through Id 
    /// method. for example:
    /// \verbatim
    /// SimpleTest request;
    /// request.set_id(  i );
    /// request.set_name( "hello world" );
    ///
    /// CommandContext context;
    /// context.sReceiver = "TestBE";
    /// context.iCommandId = CONTAINER_TEST_SIMPLE_CMD_ID;
    /// request.SerializeToString( &context.sSerializeObject );
    /// std::string result;
    /// m_pCommProxy->SyncSendCommand( &context, result );
    /// \endverbatim
    /// at the be side, you can do like this:
    /// m_pCommProxy->RegisterCommandHandler( CONTAINER_TEST_SIMPLE_CMD_ID, 
    ///       new SimpleTestCmdHandler() );
    /// you can specify the sync command timeout by iWaitTime parameter 

    struct CommandContext
    {
        std::string sReceiver;      /// the container name of receiver
        std::string sSender;        /// reserved for forward compatible, just keep empty 
        int iCommandId;             /// the id of command, make sure it's unique
        /// the marshaled object in most case by protocol buffer will send as command parameter 
        std::string sSerializeObject; 
        /// callback function that will be invoke when asynchronous command complete.
        ICommandCallbackHandler* pCommandCallback; 
        /// if 0, synchronous command will block until response return, other wait for specify time,
        /// unit is ms. if timer out command will return non-zero,and result will drop by net base. 
        /// only for sync command
        unsigned int iWaitTime;     

        /// set flag to true if you want this command request in 
        /// separate thread in service side 
        // [deprecated: will delete in future]
//     private:
//         bool bServiceAsyncDispatch; 
//         
//    public:
        CommandContext() : sReceiver(""), sSender(""), iCommandId(0),
            sSerializeObject(""), pCommandCallback(NULL),
             iWaitTime(0)
        {
        }
    
    };

    // add some new member to support send response later when handler command [2/2/2012 ruifei.liu]
    /// \class CommandContextEx
    /// \brief
    class ICommandContextEx
    {
    public:
        virtual ~ICommandContextEx() {}
        ///get the command receiver name, generally it's your name
        virtual std::string GetReceiver() const = 0;
        ///get the command sender name
        virtual std::string GetSender() const = 0;
        ///get the command Id
        virtual int GetCommandId() const = 0;
        ///get the serialized object send from client
        virtual std::string GetSerializeObject() const = 0;
        /// send response in your code 
        /// \param   replyObject serialized object data
        /// \return  int         0 success else failed. 
        ///
        virtual int Reply(const std::string& replyObject) const = 0;
        //////////////////////////////////////////////////////////////////////////
        // The following is for C# swig wrapper help function, don't use it directly
        virtual ICommandContextEx* Clone() = 0;
        virtual void Destory() = 0;
    };


    enum EVENTTYPE
    {
        EVENTTYPE_MIN = -1,

        EVENTTYPE_DISCONNECT = 0,
        EVENTTYPE_CONNECT_ADDR,
        EVENTTYPE_RECONNECT,
        EVENTTYPE_MAX
    };

}
#endif // MCSF_NETBASE_COMMAND_CONTEXT_H_
