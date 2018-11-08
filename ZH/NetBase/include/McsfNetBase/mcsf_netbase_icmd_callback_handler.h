////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_netbase_icmd_callback_handler.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/07
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_NETBASE_ICMD_CALLBACK_HANDLER_H_
#define MCSF_NETBASE_ICMD_CALLBACK_HANDLER_H_

#include "boost/shared_ptr.hpp"

namespace Mcsf {
    
    class IAsyncResult;
    
     /// \brief the base class of asynchronous callback ,you just need to inherit 
     /// from it and override HandleReply.
     ///
    class ICommandCallbackHandler
    {
    public:
         
        /// function that will be called when asynchronous call complete
        /// 
        /// \param   const IAsyncResult * pAsyncResult         
        /// \return  int 
        virtual int HandleReply(const IAsyncResult* pAsyncResult)=0;
        virtual ~ICommandCallbackHandler( ) { }
    };

    class ICommandCallbackHandlerEx 
    {
    public:
        virtual ~ICommandCallbackHandlerEx() {}
        virtual void HandleReply(boost::shared_ptr<IAsyncResult> pAsyncResult) = 0;
    };
}

#endif // MCSF_NETBASE_ICMD_CALLBACK_HANDLER_H_