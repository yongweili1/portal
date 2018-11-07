////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_inner_cmd_handler.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/05
///
/// \brief  some command hander class that auto register by netbase
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_INNER_CMD_HANDLER_H
#define _MCSF_INNER_CMD_HANDLER_H

#include "boost/shared_ptr.hpp"

#include "mcsf_netbase_interface.h"

namespace Mcsf {

    class CheckCastCmdHandler : public ICommandHandler 
    {
    public:
        virtual ~CheckCastCmdHandler() {};
        virtual int HandleCommand(const CommandContext* pContext,
            std::string* pReplyObject);
    };

    /// \class SendDataCmdHandler
    /// \brief process send data command to receive raw data transfer. 
    class SendDataCmdHandler : public ICommandHandlerEx
    {
    public:
        explicit SendDataCmdHandler( IDataHandler* pHandler );
        explicit SendDataCmdHandler( boost::shared_ptr<IDataHandlerEx> pHandler );

        void HandleCommand(boost::shared_ptr<ICommandContextEx> pContext);

        virtual void ReceiveData(void);
        SendDataCmdHandler* Clone()
        {
            return new SendDataCmdHandler(*this);
        }
    private:
        IDataHandler* m_pDataHandler;
        boost::shared_ptr<IDataHandlerEx> m_pDataHandlerEx;
        boost::shared_ptr<ICommandContextEx> m_pContext;
    };

    /// \class EchoCmdHandler 
    /// \brief echo command handler to simple test , just like Unix echo command.
    class EchoCmdHandler : public ICommandHandler 
    {
        virtual int HandleCommand( const CommandContext* pContext,
            std::string* pReplyObject );
    };

}

#endif