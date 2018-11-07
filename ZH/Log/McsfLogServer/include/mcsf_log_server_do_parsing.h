//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfLogServer
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2012
///  All rights reserved.
///  \author        Bo Han 
///  \email         mailto:bo.han.ext@united-imaging.com
///  \file          mcsf_log_server_do_parsing.h
///  \brief         parse the Mcsf.logger server configuration file
///  \date          August. 04, 2012
///
///   \revised by  
///   \date   
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef MCSF_LOG_DO_PARSING_H
#define MCSF_LOG_DO_PARSING_H

#include <string>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace boost::filesystem;
namespace fs = boost::filesystem;
namespace Mcsf {

	/////////////////////////////////////////////////////////////////
	///	\brief: struct for storing the configuration of logger server.
	/////////////////////////////////////////////////////////////////
	struct ServerConfig {

		// correspond to "WriteLogPort" item in the logger configuration file
		std::string m_strWritePort;

		// correspond to "ReadLogPort" item in the logger configuration file
		std::string m_strReadPort;

		// correspond to "ShowLog" item in the logger configuration file
		std::string m_bShowLog;

		// correspond to "SaveToErrorLogFile" item in the logger configuration file
		std::string m_bSaveToErrorFile;

		// correspond to "SaveDays" item in the logger configuration file
		std::string m_uSaveDays;

		// correspond to "ErrorHandlerPort" item in the logger configuration file
		std::string m_strErrorHandlerPort;

		// correspond to "LoggerViewerPort" item in the logger configuration file
		std::string m_strLogViewerPort;

		// correspond to "LogFileSize" item in the logger configuration file
		std::string m_uLogFileSize;
	};

	/////////////////////////////////////////////////////////////////
	///	\brief: retrieve the configuration of logger server and store
	///         the information into and ServerConfig object
	///	\param:
	///			[in] const std::string& strConfigFile: 
	///					the path of server configuration file
	///			[out] Mcsf::ServerConfig& server_config: 
	///                 struct for storing logger server configuration
	///	\return: true: retrieve and store configuration info successfully
	///          false: fail to retrieve and store configuration
	/////////////////////////////////////////////////////////////////
	bool ParseServerConfig(const std::string &strConfigFile, 
		Mcsf::ServerConfig &server_config);

	/////////////////////////////////////////////////////////////////
	///	\brief: Modify configuration file of logger server
	///	\param:
	///			[in] const std::string &strConfigFile: 
	///					the path of server configuration file
	///			[in] const std::string &strElemName
	///                 struct for storing logger server configuration
	///         [in] const std::string &strNewElemValue: 
	///                 struct for storing logger server configuration
	///	\return: true: Modify the configuration of logger server successfully
	///          false: fail to modify
	/////////////////////////////////////////////////////////////////
	bool ModifyServerConfig(const std::string &strConfigFile, 
		const std::string &strElemName,
		const std::string &strNewElemValue);

    /////////////////////////////////////////////////////////////////
    ///	\brief: get log header info from xml file
    ///	\param:
    ///			[in] const std::string &strConfigFile: 
    ///					the path of xml file
    ///			[in] char* tag value 
    ///					the path of xml file
    ///         [out] const std::string &strLogHeaderInfo: 
    ///                 store the log header info
    ///	\return: true for success, while false for failure.
    /////////////////////////////////////////////////////////////////
    bool GetLogHeaderInfoFromXml(const std::string &strConfigFile, char* pElemName, std::string &strElemContent);

    /////////////////////////////////////////////////////////////////
    ///	\brief: get xml Field: SoftwareFullVersion  from Xml FolderPATH: ..\UIH\appdata\version\
    ///	\param:
    ///			[in] const std::string& strXmlDir: 
    ///					the path of xml file
    ///			[in] const std::string& strElemName: 
    ///					the path of xml node
    ///         [out] const std::string &strElemContents: 
    ///                 splice node value of the all xml files in strXmlFolderPath
    ///	\return: true for success, while false for failure.
    /////////////////////////////////////////////////////////////////
    bool GetLogHeaderInfoFromXmlDir(const path& strXmlDir, const std::string& strElemName, std::string& strElemContents);
}

#endif