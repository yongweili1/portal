////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_netbase_ievent_handler.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/07
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_NETBASE_IEVENT_HANDLER_H_
#define MCSF_NETBASE_IEVENT_HANDLER_H_

#include <string>

namespace Mcsf {
    
    /// \class IEventHandler
    ///
    /// \brief the base class of event process class. when you hand event  
    /// you need implement this interface , and register to ICommunicationProxy
    /// according to RegisterEventHandler.
    class IEventHandler
    {
    public:
         
        /// function called when special command received.
        /// 
        /// \param   const CommandContext * pContext  Command context incoming       
        /// \param   std::string * pReplyObject  the serialized result will be return       
        /// \return  int return 0 if success else -1.
        virtual int HandleEvent(  const std::string& sSender, int iChannelId, int iEventId, 
            const std::string& sEvent)=0;
        virtual ~IEventHandler() { }
    };
}

#endif // MCSF_NETBASE_IEVENT_HANDLER_H_