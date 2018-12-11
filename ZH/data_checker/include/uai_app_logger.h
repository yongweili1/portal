//////////////////////////////////////////////////////////////////////////
/// \defgroup uAI
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2018
///  All rights reserved.
///
///  \author  qiangqiang.zhou     mailto:qiangqiang.zhou@united-imaging.com
///
///  \file    uAI App logger
///  \brief   
///
///  \version 1.0
///  \date    June 15, 2018
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef UAI_APP_LOGGER_H_
#define UAI_APP_LOGGER_H_

#include <string>
#include <sstream>//for serialize para message
#include <iostream> //for std::cout

#include "uai_app_defs.h"

BEGIN_NAMESPACE;

#ifndef UAI_LOG_UID
#define UAI_LOG_UID 0x00000000
#endif

class log_stream : public std::stringstream{
public:
    log_stream() {}
    ~log_stream() {
        printf("%s\n", this->str().c_str());
    }
};


// \brief: LOG DEV INFO
#ifndef _LOG_DEV_INFO 
#define _LOG_DEV_INFO log_stream()
#endif

// \brief: LOG DEV WARNING
#ifndef _LOG_DEV_WARNING 
#define _LOG_DEV_WARNING  log_stream()
#endif

// \brief: LOG DEV ERROR
#ifndef _LOG_DEV_ERROR  
#define _LOG_DEV_ERROR   log_stream()
#endif




#ifndef PRINTF_WITH_LINE_DEBUG
#ifdef _DEBUG
#define PRINTF_WITH_LINE_DEBUG(format, ...) do {                                                  \
	printf("File: %s, \nLine: %d, \tFunction:%s\n", __FILE__, __LINE__, __FUNCTION__);    \
	printf(format, ##__VA_ARGS__);                                                          \
} while (0)
#else  
#define PRINTF_WITH_LINE_DEBUG(format, ...);//do nothing for release version.
#endif
#endif

#ifndef PRINTF_WITH_LINE
#define PRINTF_WITH_LINE(format, ...) do {                                                  \
	printf("File: %s, \nLine: %d, \tFunction:%s\n", __FILE__, __LINE__, __FUNCTION__);    \
	printf(format, ##__VA_ARGS__);                                                          \
} while (0)
#endif

#ifndef PRINTF_LINE
#define PRINTF_LINE(format, ...) do {                                                  \
    printf(format, ##__VA_ARGS__);                                                          \
} while (0)
#endif

END_NAMESPACE;
#endif