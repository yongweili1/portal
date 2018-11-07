// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef mcsf_log_server_stdafx_h__
#define mcsf_log_server_stdafx_h__


#include <mcsf_config.h>

#if defined MCSF_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
	#include "targetver.h"
	#include <stdio.h>
	#include <tchar.h>

#endif 
#include "mcsf_log_processor.h"

#endif // stdafx_h__
