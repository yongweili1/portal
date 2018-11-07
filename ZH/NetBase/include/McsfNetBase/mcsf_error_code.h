////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_error_code.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/11/28
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_ERROR_CODE_H_
#define MCSF_ERROR_CODE_H_

#include <string>
#include "mcsf_netbase_export.h"

#ifdef SWIG
#define McsfNetBase_Export
#endif

namespace Mcsf {
    const int McsfReactorRemoveHandler = -1;
    
    //every time you update enum, you have to update the ExplainErrorCode function
    enum NetBaseErrorCode
    {
        McsfUndefineError = -1,

        McsfSuccess = 0,
        McsfErrorCodeStart,
        
        McsfSocketConnectFailed,
        McsfSocketListenFailed,
        McsfSocketAcceptFailed,
        McsfRegisterReactorFailed,

        McsfQueryNameCommandFailed,

        McsfSendCommandTimeout,
        McsfCommandCallReturnNULL,
        McsfSendDataFailed,

        McsfResponseSayFailed,
        McsfNotRegisterCommandHandler,
        McsfHandlerHasException,
        McsfContaineeNotRegistedInDispatcher,
        McsfNotCallStartListener,
        McsfNotImplemnentForDeprecated,

        McsfErrorCodeEnd
    };

    class McsfNetBase_Export ErrorCodeUtil 
    {
    public: 
        static std::string ExplainErrorCode(int iCode);
    };
}


#endif // MCSF_ERROR_CODE_H_

