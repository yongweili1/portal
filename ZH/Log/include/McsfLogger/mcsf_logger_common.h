//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfLogClient
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2011
///  All rights reserved.
///  \author        lei.sun 
///  \email         mailto:lei.sun@united-imaging.com
///  \file          mcsf_log_xml_parse.h
///  \brief         parse the log client xml configuration file
///  \date          July 2 , 2012
///  \commented by  xiaotao yang
///
//////////////////////////////////////////////////////////////////////////

#ifndef mcsf_logger_common_h__
#define mcsf_logger_common_h__
#include "mcsf_log_common.h"

#if defined(MCSF_PLATFORM_WIN32)
#pragma warning(disable:4127)
#endif
#ifdef MCSFLOGGER_EXPORTS
#define MCSF_LOGGER_API  MCSF_Export_Flag
#else
#define MCSF_LOGGER_API  MCSF_Import_Flag
#endif



#endif // mcsf_logger_common_h__