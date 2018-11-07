//////////////////////////////////////////////////////////////////////////
/// \defgroup 
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2011
///  All rights reserved.
///  \author        lei.sun 
///  \email         mailto:xiaotao.yang@united-imaging.com
///  \file          mcsf_logger.h
///  \brief         
///  \date          July 2 , 2012
///  \commented by  xiaotao yang
///
//////////////////////////////////////////////////////////////////////////
#ifndef mcsf_logger_rt_h__
#define mcsf_logger_rt_h__

#ifndef MCSF_PLATFORM_MR_REALSYS
#define MCSF_PLATFORM_MR_REALSYS
#endif

#include "mcsf_logger_base.h"
#if defined(MCSF_PLATFORM_WIN32)
#pragma warning(disable:4127)
#endif

#ifdef CONST_LOGUID_ZERO
#undef CONST_LOGUID_ZERO
#define CONST_LOGUID_ZERO		0x0
#else
#define CONST_LOGUID_ZERO		0x0
#endif

#ifdef LOGGER_SOURCE_NAME
#undef LOGGER_SOURCE_NAME
#define LOGGER_SOURCE_NAME		"DefaultLoggerSource"
#else
#define LOGGER_SOURCE_NAME		"DefaultLoggerSource"
#endif

#ifdef USER_DEFINED_NAMESPACE_RT
#undef USER_DEFINED_NAMESPACE_RT
#endif
#define USER_DEFINED_NAMESPACE_RT Mcsf_Rt

//////////////////////////////////////////////////////
/// \brief check if needed to launch the local transfer
/// \param: void
/// \ retval: true for need, false for no need.
//////////////////////////////////////////////////////
#ifndef LOG_IS_LOCAL_TRANSFER_TO_BE_LAUNCHED_RT 
#define LOG_IS_LOCAL_TRANSFER_TO_BE_LAUNCHED_RT() USER_DEFINED_NAMESPACE_RT::GLogIsLocalTransferTobeLaunched();
#endif

//////////////////////////////////////////////////////////////////
/// \brief      Initialize the logger instance with configuration 
///                        file. 
///        
/// \param[in]  string sConfigXML: path and name of the configuration file
///              
///             Example: ClientConfiguration.xml
///                   or D:UIH\\Config\\ClientConfiguration.xml
///  \To User:  i.  This Marco should be used at the beginning of Process.
///             ii. One Process should and need use it once.
///             
/////////////////////////////////////////////////////////////////
#ifndef LOG_LOAD_CONFIG_RT
#define LOG_LOAD_CONFIG_RT(sConfigXMLFile)  USER_DEFINED_NAMESPACE_RT::GLogLoadConfig(sConfigXMLFile);
#endif

#ifndef LOG_TRACE_INFO_ON_RT
#define LOG_TRACE_INFO_ON_RT()  USER_DEFINED_NAMESPACE::GSetLogTraceInfo(true);
#endif

#ifndef LOG_TRACE_INFO_OFF_RT
#define LOG_TRACE_INFO_OFF_RT()  USER_DEFINED_NAMESPACE::GSetLogTraceInfo(false);
#endif

#ifndef LOG_TRACE_WARNING_ON_RT
#define LOG_TRACE_WARNING_ON_RT()  USER_DEFINED_NAMESPACE::GSetLogTraceWarning(true);
#endif

#ifndef LOG_TRACE_WARNING_OFF_RT
#define LOG_TRACE_WARNING_OFF_RT()  USER_DEFINED_NAMESPACE::GSetLogTraceWarning(false);
#endif

#ifndef LOG_TRACE_ERROR_ON_RT
#define LOG_TRACE_ERROR_ON_RT()  USER_DEFINED_NAMESPACE::GSetLogTraceError(true);
#endif

#ifndef LOG_TRACE_ERROR_OFF_RT
#define LOG_TRACE_ERROR_OFF_RT()  USER_DEFINED_NAMESPACE::GSetLogTraceError(false);
#endif

// bOutputFlag  true-Write on Console; false-Write on Socket(Default Setting)
#ifndef LOG_SET_OUTPUTFLAG_RT
#define LOG_SET_OUTPUTFLAG_RT(bOutPutFlag)  USER_DEFINED_NAMESPACE_RT::GSetOutputFlag(bOutPutFlag);
#endif

#ifndef LOG_SET_SOURCE_RT
#define LOG_SET_SOURCE_RT(sSource)          USER_DEFINED_NAMESPACE_RT::GLogSetSource(sSource);
#endif

/////////////////////////////////////////////////////////////////
///    \brief: Change Trace Level
///    \param: int iLevel
///    \       iLevel 0   None of TRACE Level is opened.       
///    \              1  LOG_TRACE_FATAL, LOG_TRACE_ERROR are opened
///    \              2  LOG_TRACE_FATAL, LOG_TRACE_ERROR LOG_TRACE_WARN are opened
///    \              3  LOG_TRACE_FATAL, LOG_TRACE_ERROR 
///                      LOG_TRACE_WARN,  LOG_TRACE_INFO are opened.
///        
///    \return: True if the success.                          
///                     
////////////////////////////////////////////////////////////////////
#ifndef LOG_SET_TRACE_LEVEL_RT
#define LOG_SET_TRACE_LEVEL_RT(iLevel)   USER_DEFINED_NAMESPACE_RT::GSetTraceLevel(iLevel);
#endif


/////////////////////////////////////////////////////////////////
///    \brief: Get Trace Level
///    \param: int iLevel
///    \       iLevel 0  None of TRACE Level is opened.       
///    \              1  LOG_TRACE_FATAL, LOG_TRACE_ERROR are opened
///    \              2  LOG_TRACE_FATAL, LOG_TRACE_ERROR LOG_TRACE_WARN are opened
///    \              3  LOG_TRACE_FATAL, LOG_TRACE_ERROR 
///                      LOG_TRACE_WARN,  LOG_TRACE_INFO are opened.
///    \              others Meaningless
///    \return: True if the success.                          
///                      
/////////////////////////////////////////////////////////////////
#ifndef LOG_GET_TRACE_LEVEL_RT
#define LOG_GET_TRACE_LEVEL_RT()  USER_DEFINED_NAMESPACE_RT::GGetTraceLevel();
#endif

#ifndef LOG_IS_TRACE_INFO_ON_RT
#define LOG_IS_TRACE_INFO_ON_RT()  USER_DEFINED_NAMESPACE_RT::GIsTraceInfoOn()
#endif

#ifndef LOG_IS_TRACE_WARNING_ON_RT
#define LOG_IS_TRACE_WARNING_ON_RT()  USER_DEFINED_NAMESPACE_RT::GIsTraceWarningOn()
#endif

#ifndef LOG_IS_TRACE_ERROR_ON_RT
#define LOG_IS_TRACE_ERROR_ON_RT()  USER_DEFINED_NAMESPACE_RT::GIsTraceErrorOn()
#endif

/////////////////////////////////////////////////////////////////
///    \brief: Return to the former status of Trace Level
///    \param: 
///         
///    \return:                       
///                      
/////////////////////////////////////////////////////////////////
#ifndef LOG_RESET_TRACE_LEVEL_RT
#define LOG_RESET_TRACE_LEVEL_RT()   USER_DEFINED_NAMESPACE_RT::GResetTraceLevel();
#endif


/////////////////////////////////////////////////////////////////
/// \brief      New Logger Interface with one parameter.
///        
/// \param[in]  uiLogUID unsigned long.
///             recommend format: 
///                 begin with 0x, add sixteen numbers(HEX).
///             Example: 0x000125fa
///  \To User: i. Before use this macro, firstly use LOG_SET_SOURCE Marco 
///                 to set source.
///            ii.
/////////////////////////////////////////////////////////////////
#ifndef LOG_SVC_INFO_RT
#define LOG_SVC_INFO_RT(uiLogUID)       LOG_SVC_INFO_2_RT(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_SVC_WARNING_RT
#define LOG_SVC_WARNING_RT(uiLogUID)    LOG_SVC_WARNING_2_RT(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_SVC_ERROR_RT
#define LOG_SVC_ERROR_RT(uiLogUID)      LOG_SVC_ERROR_2_RT(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_DEV_INFO_RT
#define LOG_DEV_INFO_RT(uiLogUID)       LOG_DEV_INFO_2_RT(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_DEV_WARNING_RT
#define LOG_DEV_WARNING_RT(uiLogUID)    LOG_DEV_WARNING_2_RT(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_DEV_ERROR_RT
#define LOG_DEV_ERROR_RT(uiLogUID)      LOG_DEV_ERROR_2_RT(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_MANUAL_ENTRY_RT
#define LOG_MANUAL_ENTRY_RT(uiLogUID)   LOG_MANUAL_ENTRY_2_RT(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_TRACE_INFO_RT
#define LOG_TRACE_INFO_RT()            LOG_TRACE_INFO_2_RT(LOGGER_SOURCE_NAME)
#endif

#ifndef LOG_TRACE_WARN_RT
#define LOG_TRACE_WARN_RT()            LOG_TRACE_WARN_2_RT(LOGGER_SOURCE_NAME)
#endif

#ifndef LOG_TRACE_ERROR_RT
#define LOG_TRACE_ERROR_RT()           LOG_TRACE_ERROR_2_RT(LOGGER_SOURCE_NAME)
#endif

#ifndef LOG_USER_DEFINED_2_RT
#define LOG_USER_DEFINED_2_RT(sSource, uiLogUID, sLogLevel, sTime, sFileName, sFunctionName, iLineNum)    \
	LOG_USER_DEFINED_2_RT(sSource, uiLogUID, sLogLevel, sTime, sFileName, sFunctionName, iLineNum)  
#endif



/////////////////////////////////////////////////////////////////////
/// \brief      New Logger Interface with two parameter.
///        
/// \param[in]  src      source --- Which module the log come from
///             uiLogUID unsigned int.
///             recommend format:
///                 Src recommend format:  
///                         begin with SW, add module name,such as SW_Logger 
///                 LogUID recommend format:
///                         begin with 0x, add eight numbers(HEX).
///             Example: (SW_Container, 0x000125fa)
///           
/////////////////////////////////////////////////////////////////////
#ifndef LOG_SVC_INFO_2_RT
#define LOG_SVC_INFO_2_RT(sSource, uiLogUID)        USER_DEFINED_NAMESPACE_RT::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_SVC_INFO")
#endif

#ifndef LOG_SVC_WARNING_2_RT
#define LOG_SVC_WARNING_2_RT(sSource, uiLogUID)     USER_DEFINED_NAMESPACE_RT::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_SVC_WARNING")
#endif

#ifndef LOG_SVC_ERROR_2_RT
#define LOG_SVC_ERROR_2_RT(sSource, uiLogUID)       USER_DEFINED_NAMESPACE_RT::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_SVC_ERROR")
#endif

#ifndef LOG_DEV_INFO_2_RT
#define LOG_DEV_INFO_2_RT(sSource, uiLogUID)        USER_DEFINED_NAMESPACE_RT::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_DEV_INFO")
#endif

#ifndef LOG_DEV_WARNING_2_RT
#define LOG_DEV_WARNING_2_RT(sSource, uiLogUID)     USER_DEFINED_NAMESPACE_RT::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_DEV_WARNING")
#endif

#ifndef LOG_DEV_ERROR_2_RT
#define LOG_DEV_ERROR_2_RT(sSource, uiLogUID)       USER_DEFINED_NAMESPACE_RT::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_DEV_ERROR")
#endif

#ifndef LOG_MANUAL_ENTRY_2_RT
#define LOG_MANUAL_ENTRY_2_RT(sSource, uiLogUID)    USER_DEFINED_NAMESPACE_RT::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_MANUAL_ENTRY")//TODO
#endif

#ifndef LOG_TRACE_INFO_2_RT
#define LOG_TRACE_INFO_2_RT(sSource)    USER_DEFINED_NAMESPACE_RT::CLogTraceInfo(sSource, CONST_LOGUID_ZERO, __FILE__, __FUNCTION__, __LINE__, "LOG_TRACE_INFO")
#endif

#ifndef LOG_TRACE_WARN_2_RT
#define LOG_TRACE_WARN_2_RT(sSource)    USER_DEFINED_NAMESPACE_RT::CLogTraceWarning(sSource, CONST_LOGUID_ZERO, __FILE__, __FUNCTION__, __LINE__, "LOG_TRACE_WARN")
#endif

#ifndef LOG_TRACE_ERROR_2_RT
#define LOG_TRACE_ERROR_2_RT(sSource)   USER_DEFINED_NAMESPACE_RT::CLogTraceError(sSource, CONST_LOGUID_ZERO, __FILE__, __FUNCTION__, __LINE__, "LOG_TRACE_ERROR")
#endif

#ifndef LOG_USER_DEFINED_2_RT
#define LOG_USER_DEFINED_2_RT(sSource, uiLogUID, sLogLevel, sTime, sFileName, sFunctionName, iLineNum)   \
	USER_DEFINED_NAMESPACE_RT::CLog(sSource, uiLogUID, sFileName , sFunctionName, iLineNum, sLogLevel, sTime)//TODO
#endif

///////////////////////////////////////////
//excute_time
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef LOG_EXECUTING_TIME_RT
#define LOG_EXECUTING_TIME_RT  USER_DEFINED_NAMESPACE_RT::CLogExecutingTime excute_time_by_sunlei_in_logger = USER_DEFINED_NAMESPACE_RT::CLogExecutingTime("ExecutingTime", __FILE__, __FUNCTION__, __LINE__);
#endif

#ifndef LOG_FUNCTION_DURATION_RT
#define LOG_FUNCTION_DURATION_RT  USER_DEFINED_NAMESPACE_RT::CLogExecutingTime function_excute_time_by_sunlei_in_logger = USER_DEFINED_NAMESPACE_RT::CLogExecutingTime("FUNCTION_DURATION", __FILE__, __FUNCTION__, __LINE__);
#endif

#ifndef LOG_BLOCK_DURATION_RT
#define LOG_BLOCK_DURATION_RT(block_name)  USER_DEFINED_NAMESPACE_RT::CLogExecutingTime block_excute_time_by_sunlei_in_logger = USER_DEFINED_NAMESPACE_RT::CLogExecutingTime("BLOCK_DURATION", __FILE__, __FUNCTION__, __LINE__, block_name);
#endif
//TODO LOG_FUNCTION_TRACE
//LOG_BLOCK_TRACE


#define LOG_DUMP_TO_FILE_RT() USER_DEFINED_NAMESPACE_RT::CLogDumpToFile()
/////////////////////////////////////////////////////////////////////
/// \brief  ERASE_LOGGER Instance 
///        
/// \  i. Delete Logger instance
/// \  ii.Other function of this Marco is to make sure all logs
///        are send to the log Server before the main thread quit.           
/// \  iii.This Marco should be used in the end of the main thread quit.             
///                 
//////////////////////////////////////////////////////////////////////////
#ifndef ERASE_LOGGER_RT
#define ERASE_LOGGER_RT            USER_DEFINED_NAMESPACE_RT::GLogUnloadConfig();
#endif

#ifndef LOG_UNLOAD_CONFIG_RT
#define LOG_UNLOAD_CONFIG_RT()     USER_DEFINED_NAMESPACE_RT::GLogUnloadConfig();
#endif

///return std::string as service code
#ifndef GET_SERVICE_CODE_RT
#define GET_SERVICE_CODE_RT(iLogUID) USER_DEFINED_NAMESPACE_RT::GGetServiceCode(iLogUID)
#endif

///return uint64_t as LOGUID
#ifndef MAKE_LOGUID_RT
#define MAKE_LOGUID_RT(iBU, iModality, iErrorLevel, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE_RT::GMakeLogUID(iBU, iModality, iErrorLevel, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

//MakeLogUID for level
#ifndef MAKE_LOGUID_SERVICE_ERROR_RT
#define MAKE_LOGUID_SERVICE_ERROR_RT(iBU, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE_RT::GMakeLogUID(iBU, iModality, 0, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SERVICE_WARNING_RT
#define MAKE_LOGUID_SERVICE_WARNING_RT(iBU, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE_RT::GMakeLogUID(iBU, iModality, 1, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SERVICE_INFO_RT
#define MAKE_LOGUID_SERVICE_INFO_RT(iBU, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE_RT::GMakeLogUID(iBU, iModality, 2, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_DEV_ERROR_RT
#define MAKE_LOGUID_DEV_ERROR_RT(iBU, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE_RT::GMakeLogUID(iBU, iModality, 3, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_DEV_WARNING_RT
#define MAKE_LOGUID_DEV_WARNING_RT(iBU, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE_RT::GMakeLogUID(iBU, iModality, 4, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_DEV_INFO_RT
#define MAKE_LOGUID_DEV_INFO_RT(iBU, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE_RT::GMakeLogUID(iBU, iModality, 5, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

//MakeLogUID for SW && LEVEL
#ifndef MAKE_LOGUID_SW_SERVICE_ERROR_RT
#define MAKE_LOGUID_SW_SERVICE_ERROR_RT(iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_ERROR_RT(0x00, 0x00, 0x00, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SW_SERVICE_WARNING_RT
#define MAKE_LOGUID_SW_SERVICE_WARNING_RT(iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_WARNING_RT(0x00, 0x00, 0x00, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SW_SERVICE_INFO_RT
#define MAKE_LOGUID_SW_SERVICE_INFO_RT(iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_INFO_RT(0x00, 0x00, 0x00, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SW_DEV_ERROR_RT
#define MAKE_LOGUID_SW_DEV_ERROR_RT(iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_ERROR_RT(0x00, 0x00, 0x00, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SW_DEV_WARNING_RT
#define MAKE_LOGUID_SW_DEV_WARNING_RT(iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_WARNING_RT(0x00, 0x00, 0x00, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SW_DEV_INFO_RT
#define MAKE_LOGUID_SW_DEV_INFO_RT(iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_INFO_RT(0x00, 0x00, 0x00, iLogicalComponent, iSpecialCode)
#endif

//MakeLogUID for XR && LEVEL
#ifndef MAKE_LOGUID_XR_SERVICE_ERROR_RT
#define MAKE_LOGUID_XR_SERVICE_ERROR_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_ERROR_RT(0x01, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_XR_SERVICE_WARNING_RT
#define MAKE_LOGUID_XR_SERVICE_WARNING_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_WARNING_RT(0x01, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_XR_SERVICE_INFO_RT
#define MAKE_LOGUID_XR_SERVICE_INFO_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_INFO_RT(0x01, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_XR_DEV_ERROR_RT
#define MAKE_LOGUID_XR_DEV_ERROR_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_ERROR_RT(0x01, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_XR_DEV_WARNING_RT
#define MAKE_LOGUID_XR_DEV_WARNING_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_WARNING_RT(0x01, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_XR_DEV_INFO_RT
#define MAKE_LOGUID_XR_DEV_INFO_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_INFO_RT(0x01, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

//MakeLogUID for CT && LEVEL
#ifndef MAKE_LOGUID_CT_SERVICE_ERROR_RT
#define MAKE_LOGUID_CT_SERVICE_ERROR_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_ERROR_RT(0x02, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_CT_SERVICE_WARNING_RT
#define MAKE_LOGUID_CT_SERVICE_WARNING_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_WARNING_RT(0x02, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_CT_SERVICE_INFO_RT
#define MAKE_LOGUID_CT_SERVICE_INFO_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_INFO_RT(0x02, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_CT_DEV_ERROR_RT
#define MAKE_LOGUID_CT_DEV_ERROR_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_ERROR_RT(0x02, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_CT_DEV_WARNING_RT
#define MAKE_LOGUID_CT_DEV_WARNING_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_WARNING_RT(0x02, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_CT_DEV_INFO_RT
#define MAKE_LOGUID_CT_DEV_INFO_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_INFO_RT(0x02, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

//MakeLogUID for MR && LEVEL
#ifndef MAKE_LOGUID_MR_SERVICE_ERROR_RT
#define MAKE_LOGUID_MR_SERVICE_ERROR_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_ERROR_RT(0x03, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MR_SERVICE_WARNING_RT
#define MAKE_LOGUID_MR_SERVICE_WARNING_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_WARNING_RT(0x03, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MR_SERVICE_INFO_RT
#define MAKE_LOGUID_MR_SERVICE_INFO_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_INFO_RT(0x03, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MR_DEV_ERROR_RT
#define MAKE_LOGUID_MR_DEV_ERROR_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_ERROR_RT(0x03, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MR_DEV_WARNING_RT
#define MAKE_LOGUID_MR_DEV_WARNING_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_WARNING_RT(0x03, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MR_DEV_INFO_RT
#define MAKE_LOGUID_MR_DEV_INFO_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_INFO_RT(0x03, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

//MakeLogUID for MI && LEVEL
#ifndef MAKE_LOGUID_MI_SERVICE_ERROR_RT
#define MAKE_LOGUID_MI_SERVICE_ERROR_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_ERROR_RT(0x04, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MI_SERVICE_WARNING_RT
#define MAKE_LOGUID_MI_SERVICE_WARNING_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_WARNING_RT(0x04, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MI_SERVICE_INFO_RT
#define MAKE_LOGUID_MI_SERVICE_INFO_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_INFO_RT(0x04, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MI_DEV_ERROR_RT
#define MAKE_LOGUID_MI_DEV_ERROR_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_ERROR_RT(0x04, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MI_DEV_WARNING_RT
#define MAKE_LOGUID_MI_DEV_WARNING_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_WARNING_RT(0x04, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MI_DEV_INFO_RT
#define MAKE_LOGUID_MI_DEV_INFO_RT(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_INFO_RT(0x04, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#define MCSF_LOGUID USER_DEFINED_NAMESPACE_RT::sMcsfLogUID

///return structure McsfLogUID(storing all parts of LOGUID)
#ifndef PARSE_LOGUID_RT
#define PARSE_LOGUID_RT(iLogUID) USER_DEFINED_NAMESPACE_RT::GParseLogUID(iLogUID)
#endif

#endif // mcsf_logger_rt_h__
