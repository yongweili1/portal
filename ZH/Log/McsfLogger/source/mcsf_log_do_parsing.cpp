//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfLogLocalTransfer
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2011
///  All rights reserved.
///  \author        xiaotao yang 
///  \email         mailto:xiaotao.yang@united-imaging.com
///  \file          mcsf_log_xml_parse.h
///  \brief         parse the log client xml configuration file
///  \date          July 2 , 2012
///
//////////////////////////////////////////////////////////////////////////

#include "mcsf_log_do_parsing.h"
#include "boost/scoped_ptr.hpp"

//#ifdef MCSF_PLATFORM_WIN32
//	#ifdef _WIN64
//		static const std::string LOG_SERVER_ADDRESS_CONFIG = "../appdata/sysconfig/McsfSystemIPConfig.xml";
//	#else
//		static const std::string LOG_SERVER_ADDRESS_CONFIG = "../../appdata/sysconfig/McsfSystemIPConfig.xml";
//	#endif
//#else
//	static const std::string LOG_SERVER_ADDRESS_CONFIG = "../appdata/sysconfig/McsfSystemIPConfig.xml";
//#endif

namespace Mcsf{	

	static const std::string	LOGGER_TRACE_SWITCH = "config/sysconfig/LoggerTraceSwitchConfiguration.xml";
	/////////////////////////////////////////////////////////////////
	///	\brief: parse the configuration file to get server info.
	///	\param:
	///			[in] const std::string& filePath: the configuration file
	///         [out] Mcsf::ConfigInfo& serverInfo: restore the server info.
	///	\return: true:  success to parse configuration file,
	///          false: fail to parse configuration file.
	/////////////////////////////////////////////////////////////////
	bool ParsingServerConfig(const std::string& filePath, USER_DEFINED_NAMESPACE::ConfigInfo& serverInfo)
	{
		try
		{
			boost::scoped_ptr<CLogXmlParse> ptrLogXmlParser(new CLogXmlParse);
			if(ptrLogXmlParser->LoadConfigXmlFile(filePath))
			{
				if(!ptrLogXmlParser->GetContentByXmlTag("ServerIP",serverInfo.m_sServerIP))
				{
					//std::cerr<<"reading server ip failed!"<<std::endl;
					return false;
				}

				if(!ptrLogXmlParser->GetContentByXmlTag("LogServerPort", serverInfo.m_sServerPort))
				{
					//std::cerr<<"reading server port failed!\n"<<std::endl;
					return false;
				}

				if(!ptrLogXmlParser->GetContentByXmlTag("LOG_TRACE_ERROR",serverInfo.m_sLogTraceError))
				{
					//std::cerr<<"reading log_trace_error failed"<<std::endl;
					return false;
				}

				if(!ptrLogXmlParser->GetContentByXmlTag("LOG_TRACE_WARN", serverInfo.m_sLogTraceWarn))
				{
					//std::cerr<<"reading log_trace_warn failed!"<<std::endl;
					return false;
				}

				if(!ptrLogXmlParser->GetContentByXmlTag("LOG_TRACE_INFO", serverInfo.m_sLogTraceInfo))
				{
					//std::cerr<<"reading log_trace_info failed!"<<std::endl;
					return false;
				}

				if(!ptrLogXmlParser->GetContentByXmlTag("LOG_CLIENT_NAME", serverInfo.m_sClientName))
				{
					//std::cerr<<"reading client name failed!"<<std::endl;
					return false;
				}
			}
			else
			{
				//std::cerr<<"Parsing configuration failed !"<<std::endl;
				return false;
			}
            return true;
			//return GetLogServerIpAndPort(serverInfo);
		}
		catch(...)
		{
			std::cerr << "fail to parse configuration" << std::endl;
			return false;
		}
	}

    ////////////////////////////////////////////////////////////
    ////\brief: parse the configuration to get log server ip&port
    ////////////////////////////////////////////////////////////
//     bool GetLogServerIpAndPort(USER_DEFINED_NAMESPACE::ConfigInfo& serverInfo)
//     {
//         static std::string    s_strServerIp = "127.0.0.1";
//         static std::string    s_strServerPort = "9998";
//         static bool           s_bIsServerConfigParsed = false;
//         if(!s_bIsServerConfigParsed)
//         {
//             s_bIsServerConfigParsed = true;
//             Mcsf::ISystemEnvironmentConfig *sysEnvConfigPtr = 
//                 Mcsf::ConfigSystemEnvironmentFactory::Instance()->GetSystemEnvironment();
//             if (sysEnvConfigPtr)
//             {
//                 std::string serverEndpoint; 
//                 if (sysEnvConfigPtr->GetLoggerServerAddress(serverEndpoint))
//                 {
//                     std::string::size_type mark = serverEndpoint.find_first_of(':');
//                     if (std::string::npos != mark)
//                     {
//                         s_strServerIp = 
//                             std::string(serverEndpoint.begin()
//                             , serverEndpoint.begin() + mark);
//                         s_strServerPort =
//                             std::string(serverEndpoint.begin() + mark + 1
//                             , serverEndpoint.end());
//                     }
//                 }
//                 delete sysEnvConfigPtr;
//             }
//         }
//         serverInfo.m_sServerIP = s_strServerIp;
//         serverInfo.m_sServerPort = s_strServerPort;
//         return true;
//     }

    /////////////////////////////////////////////////////////////////
    ///	\brief: parse the configuration of Trace Switch in \appdata\logger\Config\LoggerTraceConfiguration.xml
    /////////////////////////////////////////////////////////////////
//     bool ParsingTraceSwitchConfig(USER_DEFINED_NAMESPACE::TraceSwitch& traceSwitch)
//     {
//         try
//         {
//             Mcsf::ISystemEnvironmentConfig *sysEnvConfigPtr =
//                 Mcsf::ConfigSystemEnvironmentFactory::Instance()->GetSystemEnvironment();
//             boost::shared_ptr<Mcsf::ISystemEnvironmentConfig> SysConfig(sysEnvConfigPtr);
// 
//             std::string sysConfigPath = SysConfig->GetCurrentUserSettingsDir();
//             sysConfigPath += LOGGER_TRACE_SWITCH;
// 
//             boost::scoped_ptr<CLogXmlParse> ptrLogXmlParser(new CLogXmlParse);
//             if (!ptrLogXmlParser->LoadConfigXmlFile(sysConfigPath))
//             {
//                 sysConfigPath = SysConfig->GetDefaultUserSettingsDir();
//                 sysConfigPath += LOGGER_TRACE_SWITCH;
//                 if (!ptrLogXmlParser->LoadConfigXmlFile(sysConfigPath))
//                 {
//                     sysConfigPath = SysConfig->GetFactoryDefaultSettingsDir();
//                     sysConfigPath += LOGGER_TRACE_SWITCH;
//                     if (!ptrLogXmlParser->LoadConfigXmlFile(sysConfigPath))
//                         return false;
//                 }
//             }
// 
//             if (!ptrLogXmlParser->GetContentByXmlTag("LOG_TRACE_SWITCH", traceSwitch.m_traceSwitch))
//                 return false;
// 
//             if (!ptrLogXmlParser->GetContentByXmlTag("PerformanceRecord", traceSwitch.m_performanceRecord))
//                 return false;
// 
//             if (!ptrLogXmlParser->GetContentByXmlTag("AUDIT_LOG_SWITCH", traceSwitch.m_auditLogSwitch))
//                 return false;
// 
//             return true;
//         }
//         catch (...)
//         {
//             std::cerr << "fail to parse configuration" << std::endl;
//             return false;
//         }
//     }
}
