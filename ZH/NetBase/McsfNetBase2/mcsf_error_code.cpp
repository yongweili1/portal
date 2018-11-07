#include "mcsf_error_code.h"

#define BEGIN_ERROR_TO_STRING(iCode) switch(iCode) {
#define END_ERROR_TO_STRING() default: return "undefined error code";}
#define ERROR_CODE_TO_STRING(enumCode)  \
    case enumCode:       \
    return "ErrorInfo:"#enumCode

namespace Mcsf {

    std::string ErrorCodeUtil::ExplainErrorCode(int iCode)
    {
        BEGIN_ERROR_TO_STRING(iCode)
            ERROR_CODE_TO_STRING(McsfUndefineError);
            ERROR_CODE_TO_STRING(McsfSocketConnectFailed);
            ERROR_CODE_TO_STRING(McsfSocketListenFailed);
            ERROR_CODE_TO_STRING(McsfSocketAcceptFailed);
            ERROR_CODE_TO_STRING(McsfRegisterReactorFailed);
            ERROR_CODE_TO_STRING(McsfQueryNameCommandFailed);
            ERROR_CODE_TO_STRING(McsfSendCommandTimeout);
            ERROR_CODE_TO_STRING(McsfCommandCallReturnNULL);
            ERROR_CODE_TO_STRING(McsfSendDataFailed);
            ERROR_CODE_TO_STRING(McsfResponseSayFailed);
            ERROR_CODE_TO_STRING(McsfHandlerHasException);
            ERROR_CODE_TO_STRING(McsfContaineeNotRegistedInDispatcher);
            ERROR_CODE_TO_STRING(McsfNotCallStartListener);
            ERROR_CODE_TO_STRING(McsfNotImplemnentForDeprecated);
        END_ERROR_TO_STRING()
    }
}
