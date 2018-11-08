
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl -n McsfNetBase
// Modify by ruifei.liu for don't include ace
// ------------------------------
#ifndef MCSFNETBASE_EXPORT_H
#define MCSFNETBASE_EXPORT_H

//#include "ace/config-all.h"

#if !defined (MCSFNETBASE_HAS_DLL)
#  define MCSFNETBASE_HAS_DLL 1
#endif /* ! MCSFNETBASE_HAS_DLL */

#ifndef Mcsf_Proper_Export_Flag
#ifdef WIN32
#define Mcsf_Proper_Export_Flag __declspec (dllexport)
#else
#define Mcsf_Proper_Export_Flag
#endif
#endif 

#ifndef Mcsf_Proper_Import_Flag
#ifdef WIN32
#define Mcsf_Proper_Import_Flag __declspec (dllimport)
#else
#define Mcsf_Proper_Import_Flag
#endif
#endif 

#if defined (MCSFNETBASE_HAS_DLL) && (MCSFNETBASE_HAS_DLL == 1)
#  if defined (MCSFNETBASE_BUILD_DLL)
#    define McsfNetBase_Export Mcsf_Proper_Export_Flag
//#    define MCSFNETBASE_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
//#    define MCSFNETBASE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* MCSFNETBASE_BUILD_DLL */
#    define McsfNetBase_Export Mcsf_Proper_Import_Flag
//#    define MCSFNETBASE_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
//#    define MCSFNETBASE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* MCSFNETBASE_BUILD_DLL */
#else /* MCSFNETBASE_HAS_DLL == 1 */
#  define McsfNetBase_Export
//#  define MCSFNETBASE_SINGLETON_DECLARATION(T)
//#  define MCSFNETBASE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* MCSFNETBASE_HAS_DLL == 1 */

// Set MCSFNETBASE_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.


// #if !defined (MCSFNETBASE_NTRACE)
// #  if (ACE_NTRACE == 1)
// #    define MCSFNETBASE_NTRACE 1
// #  else /* (ACE_NTRACE == 1) */
// #    define MCSFNETBASE_NTRACE 0
// #  endif /* (ACE_NTRACE == 1) */
// #endif /* !MCSFNETBASE_NTRACE */
// 
// #if (MCSFNETBASE_NTRACE == 1)
// #  define MCSFNETBASE_TRACE(X)
// #else /* (MCSFNETBASE_NTRACE == 1) */
// #  if !defined (ACE_HAS_TRACE)
// #    define ACE_HAS_TRACE
// #  endif /* ACE_HAS_TRACE */
// #  define MCSFNETBASE_TRACE(X) ACE_TRACE_IMPL(X)
// #  include "ace/Trace.h"
// #endif /* (MCSFNETBASE_NTRACE == 1) */

#endif /* MCSFNETBASE_EXPORT_H */

// End of auto generated file.
