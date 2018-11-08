//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfLogClient
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2011
///  All rights reserved.
///  \author        xiaotao yang 
///  \email         mailto:xiaotao.yang@united-imaging.com
///  \file          mcsf_log_xml_parse.h
///  \brief         parse the log client xml configuration file
///  \date          July 2 , 2012
///
//////////////////////////////////////////////////////////////////////////

#ifndef MCSF_LOG_DO_PARSING_H_
#define MCSF_LOG_DO_PARSING_H_

#include "mcsf_logger_common.h"
#include "mcsf_log_xml_parse.h"

#ifdef _WIN32
#pragma warning(disable:4819)  // disable boost warnings
#endif

namespace Mcsf{
#define USER_DEFINED_NAMESPACE Mcsf

	struct ConfigInfo{
		std::string m_sServerIP;
		std::string m_sServerPort;
		std::string m_sLogTraceError;
		std::string m_sLogTraceWarn;
		std::string m_sLogTraceInfo;
		std::string m_sAuditLogInfo;
		std::string m_sClientName;
	};
	struct TraceSwitch{
		std::string m_traceSwitch;
		std::string m_performanceRecord;
		std::string m_auditLogSwitch;
	};

	MCSF_LOGGER_API bool ParsingServerConfig(const std::string& filePath, USER_DEFINED_NAMESPACE::ConfigInfo& serverInfo);
    //MCSF_LOGGER_API bool GetLogServerIpAndPort(USER_DEFINED_NAMESPACE::ConfigInfo& serverInfo);
	//MCSF_LOGGER_API bool ParsingTraceSwitchConfig(USER_DEFINED_NAMESPACE::TraceSwitch& traceSwitch);
}

#endif