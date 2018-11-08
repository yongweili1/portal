////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_netbase_command_handler.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/07
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_NETBASE_COMMAND_HANDLER_H_
#define MCSF_NETBASE_COMMAND_HANDLER_H_

#include <string>

#include "boost/shared_ptr.hpp"

#include "mcsf_netbase_command_context.h"

namespace Mcsf {
        
    /// \class ICommandHandler
    ///
    /// \brief the base class of command process class. when you process command 
    /// you need implement this interface , and register to ICommunicationProxy
    /// according to RegisterCommandHandler.
    class ICommandHandler
    {
    public:
         
        /// function called when special command received.
        /// 
        /// \param   const CommandContext * pContext  Command context incoming       
        /// \param   std::string * pReplyObject  the serialized result will be return       
        /// \return  int return 0 if success else -1.
        virtual int HandleCommand(const CommandContext* pContext,
            std::string* pReplyObject)=0;
        virtual ~ICommandHandler() { }
    };

    class ICommandHandlerEx
    {
    public:
        virtual void HandleCommand(boost::shared_ptr<ICommandContextEx> pContext) = 0;
        virtual ~ICommandHandlerEx() {}
    };
}

#endif // MCSF_NETBASE_COMMAND_HANDLER_H_
