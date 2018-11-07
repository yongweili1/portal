#include "stdafx.h"

//#include "mcsf_asyncResult.h"

namespace Mcsf {

    AsyncResult::AsyncResult() 
        : m_bComplete(false)
        , m_pReponseCallback(NULL)
        , m_sSender("")
    {
        CErrorHandler::RegisterToDump(this);
    }

    AsyncResult::~AsyncResult()
    {
        CErrorHandler::UnRegisterToDump(this);
    }

    std::string AsyncResult::Dump()
    {
        MCSF_TRY
            MCSF_DUMP(m_sDumpInfo, "(void)*", "this:", this );
            MCSF_DUMP(m_sDumpInfo, "ICommandCallbackHandler*", "m_pReponseCallback:", m_pReponseCallback);
            MCSF_DUMP(m_sDumpInfo, "bool", "m_bComplete:",  m_bComplete);
            MCSF_DUMP(m_sDumpInfo, "string", "m_sSender:", m_sSender);
            MCSF_DUMP(m_sDumpInfo, "enum", "m_callResult:", m_callResult);
            return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
            return "";
        MCSF_TRY_CATCH_END
    }
}