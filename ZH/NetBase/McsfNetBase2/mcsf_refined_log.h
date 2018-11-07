#ifndef __mcsf_refined_log__
#define __mcsf_refined_log__

#define USE_LOG 1 
#include "McsfLogger/mcsf_logger.h"

#include <sstream>
#define  print_log(description) do{stringstream ss; ss<<description<<std::endl; printf(ss.str().c_str());}while (false, false)

namespace Mcsf {
#ifdef USE_LOG
#define LOG_ERROR_CTN(name, id, description)                                   \
    do                                                                         \
    {                                                                          \
        LOG_DEV_ERROR_2(name, id) << description;                              \
    }                                                                          \
    while (false, false)
#define LOG_ERROR_CTN_WITH_THIS(name, id, description)                         \
    do                                                                         \
    {                                                                          \
        LOG_DEV_ERROR_2(name, id) << "this="<<this<<" "<<description;          \
    }                                                                          \
    while (false, false)

#define LOG_WARN_CTN(name, id, description)                                    \
    do                                                                         \
    {                                                                          \
        LOG_DEV_WARNING_2(name, id) << description;                            \
    }                                                                          \
    while (false, false)
#define LOG_WARN_CTN_WITH_THIS(name, id, description)                         \
    do                                                                         \
    {                                                                          \
        LOG_DEV_WARNING_2(name, id) << "this="<<this<<" "<<description;        \
    }                                                                          \
    while (false, false)

#define LOG_INFO_CTN(name, id, description)                                    \
    do                                                                         \
    {                                                                          \
        LOG_DEV_INFO_2(name, id) << description;                               \
    }                                                                          \
    while (false, false)
#define LOG_INFO_CTN_WITH_THIS(name, id, description)                         \
    do                                                                         \
    {                                                                          \
        LOG_DEV_INFO_2(name, id) << "this="<<this<<" "<<description;          \
    }                                                                          \
    while (false, false)
#else
#define LOG_ERROR_CTN(name, id, description)  print_log(description)
#define LOG_WARN_CTN(name, id, description)   print_log(description)
#define LOG_INFO_CTN(name, id, description)   print_log(description)
#endif
#define LOG_SVC_ERROR_CTN(name, id, description)                                \
    do                                                                         \
    {                                                                          \
        LOG_SVC_ERROR_2(name, id) << description;                              \
    }                                                                          \
    while (false, false)

#define LOG_SVC_WARN_CTN(name, id, description)                                \
    do                                                                         \
    {                                                                          \
        LOG_SVC_WARNING_2(name, id) << description;                            \
    }                                                                          \
    while (false, false)

#define LOG_SVC_INFO_CTN(name, id, description)                                \
    do                                                                         \
    {                                                                          \
        LOG_SVC_INFO_2(name, id) << description;                               \
    }                                                                          \
    while (false, false)


#define TRACE_INFO_CTN(name, description)                                  \
    do                                                                        \
    {                                                                         \
        if (LOG_IS_TRACE_INFO_ON())                                           \
        {                                                                     \
            LOG_TRACE_INFO_2(name) << description;                            \
        }                                                                     \
    }                                                                         \
    while (false, false)
#define TRACE_INFO_CTN_WITH_THIS(name, description)                       \
    do                                                                        \
    {                                                                         \
        if (LOG_IS_TRACE_INFO_ON())                                           \
        {                                                                     \
            LOG_TRACE_INFO_2(name) << "this="<<this<<" "<< description;       \
        }                                                                     \
    }                                                                         \
    while (false, false)


#define LOG_UNKNOWN_CTN(name, id)                                              \
    do                                                                         \
    {                                                                          \
          LOG_ERROR_CTN(name, id, "unknown error.");                           \
    }                                                                          \
    while (false, false)

#undef  ASSERT_NOT_NULL
#define ASSERT_NOT_NULL(arg, ...)                                              \
    do {                                                                       \
        if (NULL == (arg)) {                                                   \
            LOG_WARN_CTN("pointjudgment", 10005, #arg << " is null!");         \
            return __VA_ARGS__;                                                \
        }                                                                      \
    } while (false, false)

    //container
#define RLOG_ERROR(description)     LOG_ERROR_CTN("container", 10003, description)
#define RLOG_WARN(description)      LOG_WARN_CTN("container", 10003, description)
#define RLOG_INFO(description)      LOG_INFO_CTN("container", 10003, description)
#define RLOG_UNKNOWN()              LOG_UNKNOWN_CTN("container", 10003)

//netbase
#define NTB_TRACE_INFO(description)      TRACE_INFO_CTN("netbase", description)
#define NTB_TRACE_INFO_WITH_THIS(description)      TRACE_INFO_CTN_WITH_THIS("netbase", description)

#define NTB_LOG_ERROR(description)     LOG_ERROR_CTN("netbase", 10004, description)
#define NTB_LOG_WARN(description)      LOG_WARN_CTN("netbase", 10004, description)
#define NTB_LOG_INFO(description)      LOG_INFO_CTN("netbase", 10004, description)
#define NTB_LOG_ERROR_WITH_THIS(description) LOG_ERROR_CTN_WITH_THIS("netbase", 10004, description)
#define NTB_LOG_WARN_WITH_THIS(description) LOG_WARN_CTN_WITH_THIS("netbase", 10004, description)
#define NTB_LOG_INFO_WITH_THIS(description) LOG_INFO_CTN_WITH_THIS("netbase", 10004, description)

//dispatcher
#define DSP_TRACE_INFO(description)      TRACE_INFO_CTN("dispatcher", description)
#define DSP_TRACE_INFO_WITH_THIS(description)      TRACE_INFO_CTN_WITH_THIS("dispatcher", description)

#define DSP_LOG_ERROR(description)     LOG_ERROR_CTN("dispatcher", 10005, description)
#define DSP_LOG_WARN(description)      LOG_WARN_CTN("dispatcher", 10005, description)
#define DSP_LOG_INFO(description)      LOG_INFO_CTN("dispatcher", 10005, description)
#define DSP_LOG_ERROR_WITH_THIS(description) LOG_ERROR_CTN_WITH_THIS("dispatcher", 10005, description)
#define DSP_LOG_WARN_WITH_THIS(description) LOG_WARN_CTN_WITH_THIS("dispatcher", 10005, description)
#define DSP_LOG_INFO_WITH_THIS(description) LOG_INFO_CTN_WITH_THIS("dispatcher", 10005, description)


}

#endif
