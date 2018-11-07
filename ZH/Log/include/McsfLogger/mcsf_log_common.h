//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2012
///  All rights reserved.
/// 
///  \author  Sun Lei (mailto:lei.sun@united-imaging.com)
/// 
///  \file mcsf.log.common.h
/// 
///  \brief definitions declaration
/// 
///  \version 1.0
/// 
///  \date    August/1/2012
//////////////////////////////////////////////////////////////////////////
#ifndef mcsf_log_common_h__
#define mcsf_log_common_h__

#if defined(MCSF_PLATFORM_AIX)
#    undef MCSF_PLATFORM_AIX
#endif
#if defined(MCSF_PLATFORM_AMIGAOS)
#    undef MCSF_PLATFORM_AMIGAOS
#endif
#if defined(MCSF_PLATFORM_ANDROID)
#    undef MCSF_PLATFORM_ANDROID
#endif
#if defined(MCSF_PLATFORM_BEOS)
#    undef MCSF_PLATFORM_BEOS
#endif
#if defined(MCSF_PLATFORM_BSD)
#    undef MCSF_PLATFORM_BSD
#endif
#if defined(MCSF_PLATFORM_CYGWIN)
#    undef MCSF_PLATFORM_CYGWIN
#endif
#if defined(MCSF_PLATFORM_HPUX)
#    undef MCSF_PLATFORM_HPUX
#endif
#if defined(MCSF_PLATFORM_IRIX)
#    undef MCSF_PLATFORM_IRIX
#endif
#if defined(MCSF_PLATFORM_LINUX)
#    undef MCSF_PLATFORM_LINUX
#endif
#if defined(MCSF_PLATFORM_MACOS)
#    undef MCSF_PLATFORM_MACOS
#endif
#if defined(MCSF_PLATFORM_MINIX)
#    undef MCSF_PLATFORM_MINIX
#endif
#if defined(MCSF_PLATFORM_UNIX)
#    undef MCSF_PLATFORM_UNIX
#endif
#if defined(MCSF_PLATFORM_QNX)
#    undef MCSF_PLATFORM_QNX
#endif
#if defined(MCSF_PLATFORM_SOLARIS)
#    undef MCSF_PLATFORM_SOLARIS
#endif
#if defined(MCSF_PLATFORM_SYMBIAN)
#    undef MCSF_PLATFORM_SYMBIAN
#endif
#if defined(MCSF_PLATFORM_TRU64)
#    undef MCSF_PLATFORM_TRU64
#endif
#if defined(MCSF_PLATFORM_VXWORKS)
#    undef MCSF_PLATFORM_VXWORKS
#endif
#if defined(MCSF_PLATFORM_WIN32)
#    undef MCSF_PLATFORM_WIN32
#endif
#if defined(MCSF_PLATFORM_POSIX)
#    undef MCSF_PLATFORM_POSIX
#endif

#if defined(__IBMCPP__) || defined(_AIX)
#    define MCSF_PLATFORM_AIX
#elif defined(AMIGA) || defined(__amigaos__)
#    define MCSF_PLATFORM_AMIGAOS
#elif defined(__ANDROID__)
#    define MCSF_PLATFORM_ANDROID
#elif defined(__BEOS__)
#    define MCSF_PLATFORM_BEOS
#elif defined(__bsdi__) || defined(__DragonFly__) || defined(__FreeBSD__) || \
	defined(__NetBSD__) || defined(__OpenBSD__)
#    define MCSF_PLATFORM_BSD
#elif defined(__CYGWIN__)
#    define MCSF_PLATFORM_CYGWIN
#elif defined(hpux) || defined(_hpux) || defined(__hpux)
#    define MCSF_PLATFORM_HPUX
#elif defined(sgi) || defined(__sgi)
#    define MCSF_PLATFORM_IRIX
#elif defined(linux) || defined(__linux) || defined(__linux__) || \
	defined(__GNU__) || defined(__GLIBC__)
#    define MCSF_PLATFORM_LINUX
#elif defined(macintosh) || defined(Macintosh) || defined(__APPLE__)
#    define MCSF_PLATFORM_MACOS
#elif defined(__minix)
#    define MCSF_PLATFORM_MINIX
#elif defined(unix) || defined(__unix) || defined(__unix__) || \
	defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
#    define MCSF_PLATFORM_UNIX
#elif defined (__QNX__) || defined(__QNXNTO__)
#    define MCSF_PLATFORM_QNX
#elif defined(sun) || defined(__sun)
#    define MCSF_PLATFORM_SOLARIS
#elif defined(__SYMBIAN32__) 
#    define MCSF_PLATFORM_SYMBIAN
#elif defined(__osf) || defined(__osf__)
#    define MCSF_PLATFORM_TRU64
#elif defined(VMS) || defined(__VMS)
#    define MCSF_PLATFORM_VMS
#elif defined(__VXWORKS__)
#    define MCSF_PLATFORM_VXWORKS
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || \
	defined(__TOS_WIN__) || defined(__WINDOWS__)
#    define MCSF_PLATFORM_WIN32
#else
#    error "Unknown platform!"
#endif

#if defined(MCSF_PLATFORM_AIX) || defined(MCSF_PLATFORM_ANDROID) || \
	defined(MCSF_PLATFORM_BSD) || defined(MCSF_PLATFORM_CYGWIN) || \
	defined(MCSF_PLATFORM_IRIX) || defined(MCSF_PLATFORM_LINUX) || \
	defined(MCSF_PLATFORM_MACOS) || defined(MCSF_PLATFORM_UNIX) || \
	defined(MCSF_PLATFORM_QNX) || defined(MCSF_PLATFORM_SOLARIS) || \
	defined(MCSF_PLATFORM_SYMBIAN) || defined(MCSF_PLATFORM_TRU64) || \
	defined(MCSF_PLATFORM_VMS)
#    define MCSF_PLATFORM_POSIX
#endif

#if defined(MCSF_Export_Flag)
#    undef MCSF_Export_Flag
#endif
#if defined(MCSF_Import_Flag)
#    undef MCSF_Import_Flag
#endif

#if   defined(MCSF_PLATFORM_WIN32)
// define export and import
#define MCSF_Export_Flag        __declspec(dllexport)
#define MCSF_Import_Flag        __declspec(dllimport)
#elif defined(MCSF_PLATFORM_POSIX)
// define export and import
#if (__GNUC__ >= 4)
#define MCSF_Export_Flag        __attribute__ ((visibility("default")))
#define MCSF_Import_Flag        __attribute__ ((visibility("default")))    // is it necessary?
#else
#define MCSF_Export_Flag
#define MCSF_Import_Flag
#endif

#else
#    error "unsupported operating system!"
#endif

#if defined(MCSF_PLATFORM_WIN32)
#pragma warning(disable:4127)
#pragma warning(disable:4251)
#endif

#if defined MCSF_PLATFORM_WIN32
#include <SDKDDKVer.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//#include <windows.h>
#endif 

#ifdef MCSFLOG_EXPORTS
#    define MCSF_LOG_API MCSF_Export_Flag
#else 
#    define MCSF_LOG_API MCSF_Import_Flag
#endif

#ifdef NS_BEGIN
#undef NS_BEGIN
#endif
#define NS_BEGIN namespace Mcsf {namespace Log{

#ifdef NS_END
#undef NS_END
#endif
#define NS_END }}

#define OUT

#define LOG_TERMINATOR '\001'//'\001'
#define LOG_SEPARATOR '\002'
#define SVC_LOG_HEAD	"LOG_SVC"
#define AUDIT_LOG_HEAD	"LOG_AUDIT_INFO"
#define TIME_SYNC_HEAD	"LOG_TIME_SYNC"
#define LOG_DUMP_FILE_NAME "../log/log_dump"
#endif // mcsf_log_common_h__
