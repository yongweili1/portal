#include "stdafx.h"

namespace Mcsf {

    int ICLRCommandHandler::HandleCommand( const CommandContext* pContext,
        std::string* pReplyObject )
    {
        SyncResult syncResult;
        int iRet = HandleCommandCLR( pContext, &syncResult );
        *pReplyObject = syncResult.GetSerializedObject();

        return iRet;
    }

    void ICLRCommandHandlerEx::HandleCommand(boost::shared_ptr<ICommandContextEx> pContext)
    {
        HandleCommandCLR(pContext.get());
    }


    void ICLRCommandCallbackHandlerEx::HandleReply( boost::shared_ptr<IAsyncResult> pAsyncResult )
    {
        HandleReplyCLR(pAsyncResult.get());
    }


    void ICLRDataHandlerEx::HandleDataTrans( boost::shared_ptr<SendDataContextEx> pDataContext )
    {
        HandleDataTransCLR(pDataContext.get());
    }

}
