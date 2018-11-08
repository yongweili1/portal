//////////////////////////////////////////////////////////////////////////
/// \defgroup 
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2011
///  All rights reserved.
///  \author        lei.sun 
///  \email         mailto:lei.sun@united-imaging.com
///  \file          mcsf_logger.h
///  \brief         
///  \date          July 2 , 2012
///  \commented by  xiaotao yang
///
//////////////////////////////////////////////////////////////////////////
#ifndef mcsf_logger_h__
#define mcsf_logger_h__

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

#ifdef USER_DEFINED_NAMESPACE
#undef USER_DEFINED_NAMESPACE
#endif
#define USER_DEFINED_NAMESPACE Mcsf

//////////////////////////////////////////////////////
/// \brief check if needed to launch the local transfer
/// \param: void
/// \ retval: true for need, false for no need.
//////////////////////////////////////////////////////
#ifndef LOG_IS_LOCAL_TRANSFER_TO_BE_LAUNCHED 
#define LOG_IS_LOCAL_TRANSFER_TO_BE_LAUNCHED() USER_DEFINED_NAMESPACE::GLogIsLocalTransferTobeLaunched();
#endif

#ifndef LOG_SET_LOCAL_LOG_PATH
#define LOG_SET_LOCAL_LOG_PATH(sLocalPath)  USER_DEFINED_NAMESPACE::GSetLocalLogPath(sLocalPath);
#endif

#ifndef LOG_SET_MACHINE_NAME
#define LOG_SET_MACHINE_NAME(sMachName)  USER_DEFINED_NAMESPACE::GSetMachineName(sMachName);
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
#ifndef LOG_LOAD_CONFIG
#define LOG_LOAD_CONFIG(sConfigXMLFile)  USER_DEFINED_NAMESPACE::GLogLoadConfig(sConfigXMLFile);
#endif

#ifndef LOG_TRACE_INFO_ON
#define LOG_TRACE_INFO_ON()  USER_DEFINED_NAMESPACE::GSetLogTraceInfo(true);
#endif

#ifndef LOG_TRACE_INFO_OFF
#define LOG_TRACE_INFO_OFF()  USER_DEFINED_NAMESPACE::GSetLogTraceInfo(false);
#endif

#ifndef LOG_TRACE_WARNING_ON
#define LOG_TRACE_WARNING_ON()  USER_DEFINED_NAMESPACE::GSetLogTraceWarning(true);
#endif

#ifndef LOG_TRACE_WARNING_OFF
#define LOG_TRACE_WARNING_OFF()  USER_DEFINED_NAMESPACE::GSetLogTraceWarning(false);
#endif

#ifndef LOG_TRACE_ERROR_ON
#define LOG_TRACE_ERROR_ON()  USER_DEFINED_NAMESPACE::GSetLogTraceError(true);
#endif

#ifndef LOG_TRACE_ERROR_OFF
#define LOG_TRACE_ERROR_OFF()  USER_DEFINED_NAMESPACE::GSetLogTraceError(false);
#endif

// bOutputFlag  true-Write on Console; false-Write on Socket(Default Setting)
#ifndef LOG_SET_OUTPUTFLAG
#define LOG_SET_OUTPUTFLAG(bOutPutFlag)  USER_DEFINED_NAMESPACE::GSetOutputFlag(bOutPutFlag);
#endif

#ifndef LOG_SET_SOURCE
#define LOG_SET_SOURCE(sSource)          USER_DEFINED_NAMESPACE::GLogSetSource(sSource);
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
#ifndef LOG_SET_TRACE_LEVEL
#define LOG_SET_TRACE_LEVEL(iLevel)   USER_DEFINED_NAMESPACE::GSetTraceLevel(iLevel);
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
#ifndef LOG_GET_TRACE_LEVEL
#define LOG_GET_TRACE_LEVEL()  USER_DEFINED_NAMESPACE::GGetTraceLevel();
#endif

#ifndef LOG_IS_TRACE_INFO_ON
#define LOG_IS_TRACE_INFO_ON()  USER_DEFINED_NAMESPACE::GIsTraceInfoOn()
#endif

#ifndef LOG_IS_TRACE_WARNING_ON
#define LOG_IS_TRACE_WARNING_ON()  USER_DEFINED_NAMESPACE::GIsTraceWarningOn()
#endif

#ifndef LOG_IS_TRACE_ERROR_ON
#define LOG_IS_TRACE_ERROR_ON()  USER_DEFINED_NAMESPACE::GIsTraceErrorOn()
#endif

#ifndef LOG_IS_PERFORMANCE_RECORD_ON
#define LOG_IS_PERFORMANCE_RECORD_ON()  USER_DEFINED_NAMESPACE::GIsPerformanceRecordOn()
#endif

/////////////////////////////////////////////////////////////////
///    \brief: Return to the former status of Trace Level
///    \param: 
///         
///    \return:                       
///                      
/////////////////////////////////////////////////////////////////
#ifndef LOG_RESET_TRACE_LEVEL
#define LOG_RESET_TRACE_LEVEL()   USER_DEFINED_NAMESPACE::GResetTraceLevel();
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
#ifndef LOG_SVC_INFO
#define LOG_SVC_INFO(uiLogUID)       LOG_SVC_INFO_2(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_SVC_WARNING
#define LOG_SVC_WARNING(uiLogUID)    LOG_SVC_WARNING_2(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_SVC_ERROR
#define LOG_SVC_ERROR(uiLogUID)      LOG_SVC_ERROR_2(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_DEV_INFO
#define LOG_DEV_INFO(uiLogUID)       LOG_DEV_INFO_2(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_DEV_WARNING
#define LOG_DEV_WARNING(uiLogUID)    LOG_DEV_WARNING_2(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_DEV_ERROR
#define LOG_DEV_ERROR(uiLogUID)      LOG_DEV_ERROR_2(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_MANUAL_ENTRY
#define LOG_MANUAL_ENTRY(uiLogUID)   LOG_MANUAL_ENTRY_2(LOGGER_SOURCE_NAME, uiLogUID)
#endif

#ifndef LOG_TRACE_INFO
#define LOG_TRACE_INFO()            LOG_TRACE_INFO_2(LOGGER_SOURCE_NAME)
#endif

#ifndef LOG_TRACE_WARN
#define LOG_TRACE_WARN()            LOG_TRACE_WARN_2(LOGGER_SOURCE_NAME)
#endif

#ifndef LOG_TRACE_ERROR
#define LOG_TRACE_ERROR()           LOG_TRACE_ERROR_2(LOGGER_SOURCE_NAME)
#endif

#ifndef LOG_USER_DEFINED
#define LOG_USER_DEFINED(sSource, uiLogUID, sLogLevel, sTime, sFileName, sFunctionName, iLineNum)    \
    LOG_USER_DEFINED_2(sSource, uiLogUID, sLogLevel, sTime, sFileName, sFunctionName, iLineNum)  
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
#ifndef LOG_SVC_INFO_2
#define LOG_SVC_INFO_2(sSource, uiLogUID)        USER_DEFINED_NAMESPACE::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_SVC_INFO")
#endif

#ifndef LOG_SVC_WARNING_2
#define LOG_SVC_WARNING_2(sSource, uiLogUID)     USER_DEFINED_NAMESPACE::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_SVC_WARNING")
#endif

#ifndef LOG_SVC_ERROR_2
#define LOG_SVC_ERROR_2(sSource, uiLogUID)       USER_DEFINED_NAMESPACE::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_SVC_ERROR")
#endif

#ifndef LOG_DEV_INFO_2
#define LOG_DEV_INFO_2(sSource, uiLogUID)        USER_DEFINED_NAMESPACE::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_DEV_INFO")
#endif

#ifndef LOG_DEV_WARNING_2
#define LOG_DEV_WARNING_2(sSource, uiLogUID)     USER_DEFINED_NAMESPACE::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_DEV_WARNING")
#endif

#ifndef LOG_DEV_ERROR_2
#define LOG_DEV_ERROR_2(sSource, uiLogUID)       USER_DEFINED_NAMESPACE::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_DEV_ERROR")
#endif

#ifndef LOG_MANUAL_ENTRY_2
#define LOG_MANUAL_ENTRY_2(sSource, uiLogUID)    USER_DEFINED_NAMESPACE::CLog(sSource, uiLogUID, __FILE__, __FUNCTION__, __LINE__, "LOG_MANUAL_ENTRY")//TODO
#endif

#ifndef LOG_TRACE_INFO_2
#define LOG_TRACE_INFO_2(sSource)    USER_DEFINED_NAMESPACE::CLogTraceInfo(sSource, CONST_LOGUID_ZERO, __FILE__, __FUNCTION__, __LINE__, "LOG_TRACE_INFO")
#endif

#ifndef LOG_TRACE_WARN_2
#define LOG_TRACE_WARN_2(sSource)    USER_DEFINED_NAMESPACE::CLogTraceWarning(sSource, CONST_LOGUID_ZERO, __FILE__, __FUNCTION__, __LINE__, "LOG_TRACE_WARN")
#endif

#ifndef LOG_TRACE_ERROR_2
#define LOG_TRACE_ERROR_2(sSource)   USER_DEFINED_NAMESPACE::CLogTraceError(sSource, CONST_LOGUID_ZERO, __FILE__, __FUNCTION__, __LINE__, "LOG_TRACE_ERROR")
#endif

#ifndef LOG_AUDIT_INFO
#define LOG_AUDIT_INFO(sSource)      USER_DEFINED_NAMESPACE::CLogAuditLog(sSource, CONST_LOGUID_ZERO, __FILE__, __FUNCTION__, __LINE__, "LOG_AUDIT_INFO")
#endif

#ifndef LOG_USER_DEFINED_2
#define LOG_USER_DEFINED_2(sSource, uiLogUID, sLogLevel, sTime, sFileName, sFunctionName, iLineNum)   \
    USER_DEFINED_NAMESPACE::CLog(sSource, uiLogUID, sFileName , sFunctionName, iLineNum, sLogLevel, sTime)//TODO
#endif
///////////////////////////////////////////
//excute_time
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef LOG_PERFORMANCE_RECORD
#define LOG_PERFORMANCE_RECORD(description)\
	do\
	{\
		if(LOG_IS_PERFORMANCE_RECORD_ON())\
			{\
				(USER_DEFINED_NAMESPACE::CLog("PerformanceRecord", 0x0005000100000001, __FILE__, __FUNCTION__, __LINE__, "LOG_DEV_INFO") << description);\
			}\
	}\
	while(false)
#endif

#ifndef LOG_EXECUTING_TIME
#define LOG_EXECUTING_TIME  USER_DEFINED_NAMESPACE::CLogExecutingTime excute_time_by_sunlei_in_logger = USER_DEFINED_NAMESPACE::CLogExecutingTime("ExecutingTime", __FILE__, __FUNCTION__, __LINE__);
#endif

#ifndef LOG_FUNCTION_DURATION
#define LOG_FUNCTION_DURATION  USER_DEFINED_NAMESPACE::CLogExecutingTime function_excute_time_by_sunlei_in_logger = USER_DEFINED_NAMESPACE::CLogExecutingTime("FUNCTION_DURATION", __FILE__, __FUNCTION__, __LINE__);
#endif

#ifndef LOG_BLOCK_DURATION
#define LOG_BLOCK_DURATION(block_name)  USER_DEFINED_NAMESPACE::CLogExecutingTime block_excute_time_by_sunlei_in_logger = USER_DEFINED_NAMESPACE::CLogExecutingTime("BLOCK_DURATION", __FILE__, __FUNCTION__, __LINE__, block_name);
#endif
//TODO LOG_FUNCTION_TRACE
//LOG_BLOCK_TRACE


#define LOG_DUMP_TO_FILE() USER_DEFINED_NAMESPACE::CLogDumpToFile()
/////////////////////////////////////////////////////////////////////
/// \brief  ERASE_LOGGER Instance 
///        
/// \  i. Delete Logger instance
/// \  ii.Other function of this Marco is to make sure all logs
///        are send to the log Server before the main thread quit.           
/// \  iii.This Marco should be used in the end of the main thread quit.             
///                 
//////////////////////////////////////////////////////////////////////////
#ifndef ERASE_LOGGER
#define ERASE_LOGGER            USER_DEFINED_NAMESPACE::GLogUnloadConfig();
#endif

#ifndef FLUSH_LOGGER
#define FLUSH_LOGGER            USER_DEFINED_NAMESPACE::GLogFlush();
#endif

#ifndef LOG_UNLOAD_CONFIG
#define LOG_UNLOAD_CONFIG()     USER_DEFINED_NAMESPACE::GLogUnloadConfig();
#endif

///return std::string as service code
#ifndef GET_SERVICE_CODE
#define GET_SERVICE_CODE(iLogUID) USER_DEFINED_NAMESPACE::GGetServiceCode(iLogUID)
#endif

///return uint64_t as LOGUID
#ifndef MAKE_LOGUID
#define MAKE_LOGUID(iBU, iModality, iErrorLevel, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE::GMakeLogUID(iBU, iModality, iErrorLevel, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

//MakeLogUID for level
#ifndef MAKE_LOGUID_SERVICE_ERROR
#define MAKE_LOGUID_SERVICE_ERROR(iBU, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE::GMakeLogUID(iBU, iModality, 0, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SERVICE_WARNING
#define MAKE_LOGUID_SERVICE_WARNING(iBU, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE::GMakeLogUID(iBU, iModality, 1, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SERVICE_INFO
#define MAKE_LOGUID_SERVICE_INFO(iBU, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE::GMakeLogUID(iBU, iModality, 2, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_DEV_ERROR
#define MAKE_LOGUID_DEV_ERROR(iBU, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE::GMakeLogUID(iBU, iModality, 3, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_DEV_WARNING
#define MAKE_LOGUID_DEV_WARNING(iBU, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE::GMakeLogUID(iBU, iModality, 4, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_DEV_INFO
#define MAKE_LOGUID_DEV_INFO(iBU, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode) \
    USER_DEFINED_NAMESPACE::GMakeLogUID(iBU, iModality, 5, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

//MakeLogUID for SW && LEVEL
#ifndef MAKE_LOGUID_SW_SERVICE_ERROR
#define MAKE_LOGUID_SW_SERVICE_ERROR(iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_ERROR(0x00, 0x00, 0x00, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SW_SERVICE_WARNING
#define MAKE_LOGUID_SW_SERVICE_WARNING(iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_WARNING(0x00, 0x00, 0x00, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SW_SERVICE_INFO
#define MAKE_LOGUID_SW_SERVICE_INFO(iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_INFO(0x00, 0x00, 0x00, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SW_DEV_ERROR
#define MAKE_LOGUID_SW_DEV_ERROR(iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_ERROR(0x00, 0x00, 0x00, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SW_DEV_WARNING
#define MAKE_LOGUID_SW_DEV_WARNING(iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_WARNING(0x00, 0x00, 0x00, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_SW_DEV_INFO
#define MAKE_LOGUID_SW_DEV_INFO(iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_INFO(0x00, 0x00, 0x00, iLogicalComponent, iSpecialCode)
#endif

//MakeLogUID for XR && LEVEL
#ifndef MAKE_LOGUID_XR_SERVICE_ERROR
#define MAKE_LOGUID_XR_SERVICE_ERROR(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_ERROR(0x01, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_XR_SERVICE_WARNING
#define MAKE_LOGUID_XR_SERVICE_WARNING(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_WARNING(0x01, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_XR_SERVICE_INFO
#define MAKE_LOGUID_XR_SERVICE_INFO(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_INFO(0x01, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_XR_DEV_ERROR
#define MAKE_LOGUID_XR_DEV_ERROR(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_ERROR(0x01, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_XR_DEV_WARNING
#define MAKE_LOGUID_XR_DEV_WARNING(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_WARNING(0x01, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_XR_DEV_INFO
#define MAKE_LOGUID_XR_DEV_INFO(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_INFO(0x01, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

//MakeLogUID for CT && LEVEL
#ifndef MAKE_LOGUID_CT_SERVICE_ERROR
#define MAKE_LOGUID_CT_SERVICE_ERROR(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_ERROR(0x02, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_CT_SERVICE_WARNING
#define MAKE_LOGUID_CT_SERVICE_WARNING(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_WARNING(0x02, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_CT_SERVICE_INFO
#define MAKE_LOGUID_CT_SERVICE_INFO(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_INFO(0x02, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_CT_DEV_ERROR
#define MAKE_LOGUID_CT_DEV_ERROR(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_ERROR(0x02, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_CT_DEV_WARNING
#define MAKE_LOGUID_CT_DEV_WARNING(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_WARNING(0x02, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_CT_DEV_INFO
#define MAKE_LOGUID_CT_DEV_INFO(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_INFO(0x02, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

//MakeLogUID for MR && LEVEL
#ifndef MAKE_LOGUID_MR_SERVICE_ERROR
#define MAKE_LOGUID_MR_SERVICE_ERROR(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_ERROR(0x03, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MR_SERVICE_WARNING
#define MAKE_LOGUID_MR_SERVICE_WARNING(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_WARNING(0x03, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MR_SERVICE_INFO
#define MAKE_LOGUID_MR_SERVICE_INFO(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_INFO(0x03, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MR_DEV_ERROR
#define MAKE_LOGUID_MR_DEV_ERROR(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_ERROR(0x03, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MR_DEV_WARNING
#define MAKE_LOGUID_MR_DEV_WARNING(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_WARNING(0x03, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MR_DEV_INFO
#define MAKE_LOGUID_MR_DEV_INFO(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_INFO(0x03, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

//MakeLogUID for MI && LEVEL
#ifndef MAKE_LOGUID_MI_SERVICE_ERROR
#define MAKE_LOGUID_MI_SERVICE_ERROR(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_ERROR(0x04, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MI_SERVICE_WARNING
#define MAKE_LOGUID_MI_SERVICE_WARNING(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_WARNING(0x04, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MI_SERVICE_INFO
#define MAKE_LOGUID_MI_SERVICE_INFO(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_SERVICE_INFO(0x04, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MI_DEV_ERROR
#define MAKE_LOGUID_MI_DEV_ERROR(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_ERROR(0x04, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MI_DEV_WARNING
#define MAKE_LOGUID_MI_DEV_WARNING(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_WARNING(0x04, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_MI_DEV_INFO
#define MAKE_LOGUID_MI_DEV_INFO(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
    MAKE_LOGUID_DEV_INFO(0x04, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

//MakeLogUID for XR && LEVEL
#ifndef MAKE_LOGUID_RT_SERVICE_ERROR
#define MAKE_LOGUID_RT_SERVICE_ERROR(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
	MAKE_LOGUID_SERVICE_ERROR(0x05, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_RT_SERVICE_WARNING
#define MAKE_LOGUID_RT_SERVICE_WARNING(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
	MAKE_LOGUID_SERVICE_WARNING(0x05, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_RT_SERVICE_INFO
#define MAKE_LOGUID_RT_SERVICE_INFO(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
	MAKE_LOGUID_SERVICE_INFO(0x05, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_RT_DEV_ERROR
#define MAKE_LOGUID_RT_DEV_ERROR(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
	MAKE_LOGUID_DEV_ERROR(0x05, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_RT_DEV_WARNING
#define MAKE_LOGUID_RT_DEV_WARNING(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
	MAKE_LOGUID_DEV_WARNING(0x05, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#ifndef MAKE_LOGUID_RT_DEV_INFO
#define MAKE_LOGUID_RT_DEV_INFO(iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)  \
	MAKE_LOGUID_DEV_INFO(0x05, iModality, iPhysicalComponent, iLogicalComponent, iSpecialCode)
#endif

#define MCSF_LOGUID USER_DEFINED_NAMESPACE::sMcsfLogUID

///return structure MCSF_LOGUID(storing all parts of LOGUID)
#ifndef PARSE_LOGUID
#define PARSE_LOGUID(iLogUID) USER_DEFINED_NAMESPACE::GParseLogUID(iLogUID)
#endif

#endif // mcsf_logger_h__
