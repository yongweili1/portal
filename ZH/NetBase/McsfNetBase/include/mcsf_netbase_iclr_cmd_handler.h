////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_netbase_iclr_cmd_handler.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/07
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_NETBASE_ICLR_CMD_HANDLER_H_
#define MCSF_NETBASE_ICLR_CMD_HANDLER_H_

#include "mcsf_netbase_command_handler.h"

namespace Mcsf{
    struct CommandContext;
    class ISyncResult;
     
    /// \class ICLRCommandHandler
    /// 
    /// \brief i have no good idea to process std::string pass by reference by SWIG, 
    /// so i wrap it in ISyncResult* by vaule.
    ///
    class ICLRCommandHandler : public ICommandHandler
    {
    public:
         
        /// virtual function have to be override in CLR code
        /// 
        /// \param   const CommandContext * pContext   Command context incoming      
        /// \param   ISyncResult * pSyncResult         Synchronous call result, CLR side call 
        ///                                            SetMarshallObject() to set result
        /// \return  int 
        virtual int HandleCommandCLR( const CommandContext* pContext, 
            ISyncResult* pSyncResult ) = 0;
         
        /// override virtual function and transfer to another interface.
        /// 
        virtual int HandleCommand(const CommandContext* pContext,
            std::string* pReplyObject);
    
    };

    class ICLRCommandHandlerEx : public ICommandHandlerEx
    {
    public:
        virtual ~ICLRCommandHandlerEx() {}

        virtual void HandleCommandCLR( const ICommandContextEx* pContext ) = 0;
        virtual void HandleCommand(boost::shared_ptr<ICommandContextEx> pContext);
    };

    class ICLRCommandCallbackHandlerEx : public ICommandCallbackHandlerEx 
    {
    public:
        virtual void HandleReply(boost::shared_ptr<IAsyncResult> pAsyncResult);
        virtual void HandleReplyCLR(const IAsyncResult* pAsyncResult) = 0;
    }; 

    class ICLRDataHandlerEx : public IDataHandlerEx
    {
    public:
        virtual void HandleDataTrans(boost::shared_ptr<SendDataContextEx> pDataContext);
        virtual void HandleDataTransCLR( const SendDataContextEx* pDataContext ) = 0;
    };

}

#endif // MCSF_NETBASE_ICLR_CMD_HANDLER_H_
