//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfLogServer
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2012
///  All rights reserved.
///  \author        Bo Han 
///  \email         mailto:bo.han.ext@united-imaging.com
///  \file          mcsf_log_server_do_parsing.cpp
///  \brief         parse the Mcsf.logger server configuration file
///  \date          August. 04, 2012
///
///   \revised by  
///   \date   
///  \{
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <boost/scoped_ptr.hpp>
#include "mcsf_log_server_do_parsing.h"
#include "mcsf_log_server_xml_parser.h"
#include "mcsf_isystemenvironment_config.h"
#include "mcsf_systemenvironment_factory.h"

using namespace std;
//#ifdef MCSF_PLATFORM_WIN32
//	#ifdef _WIN64
//		static const std::string SERVER_PORT_CONFIG_FILE = "../appdata/sysconfig/McsfSystemIPConfig.xml";
//	#else
//		static const std::string SERVER_PORT_CONFIG_FILE = "../../appdata/sysconfig/McsfSystemIPConfig.xml";
//	#endif
//#else 
//	static const std::string SERVER_PORT_CONFIG_FILE = "../appdata/sysconfig/McsfSystemIPConfig.xml";
//#endif


namespace Mcsf {
    
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
	bool ParseServerConfig(const string &strConfigFile,
						   Mcsf::ServerConfig &server_config)
	{
		try 
		{
			boost::scoped_ptr<CLogXmlParse> pServerConfigParser(new CLogXmlParse);
			if (pServerConfigParser->LoadConfigXmlFile(strConfigFile)) 
			{
				/// Configuration file exists
                
				/// if any item of the server configuration can't be read successfully
				/// it will be set to the default value
				//if (!pServerConfigParser->GetContentByXmlTag("WriteLogPort",
				//	server_config.m_strWritePort)) 
				//{
				//		server_config.m_strWritePort = "9998";
				//}

				if (!pServerConfigParser->GetContentByXmlTag("ReadLogPort",
					server_config.m_strReadPort)) 
				{
						server_config.m_strReadPort = "9999";
				}

				if (!pServerConfigParser->GetContentByXmlTag("ShowLog",
					server_config.m_bShowLog)) 
				{
						server_config.m_bShowLog = "off";
				}

				if (!pServerConfigParser->GetContentByXmlTag("SaveToErrorLogFile",
					server_config.m_bSaveToErrorFile)) 
				{
						server_config.m_bSaveToErrorFile = "off";
				}

				if (!pServerConfigParser->GetContentByXmlTag("SaveDays",
					server_config.m_uSaveDays)) 
				{
						server_config.m_uSaveDays = "30";
				}
                
				if (!pServerConfigParser->GetContentByXmlTag("ErrorHandlerPort",
					server_config.m_strErrorHandlerPort)) 
				{
						server_config.m_strErrorHandlerPort = "10013";
				}

				if (!pServerConfigParser->GetContentByXmlTag("LoggerViewerPort",
					server_config.m_strLogViewerPort)) 
				{
						server_config.m_strLogViewerPort = "11223";
				}

				if (!pServerConfigParser->GetContentByXmlTag("LogFileSize",
					server_config.m_uLogFileSize)) 
				{
						server_config.m_uLogFileSize = "10";
				}
			}
			else {
				/// Configuration file doesn't exist, ParseServerConfig Failed
				return false;
			}

			/// Read the value of the "WriteLogPort" using SystemEnvronment Lib
			///
			Mcsf::ISystemEnvironmentConfig *sysEnvConfigPtr = 
				Mcsf::ConfigSystemEnvironmentFactory::Instance()->GetSystemEnvironment();
			if (sysEnvConfigPtr)
			{
				std::string serverEndpoint;
				if (sysEnvConfigPtr->GetLoggerServerAddress(serverEndpoint))
				{
					std::string::size_type mark = serverEndpoint.find_last_of(':');
					if (std::string::npos != mark)
					{
						server_config.m_strWritePort = std::string(serverEndpoint.begin()+mark+1, serverEndpoint.end());
						delete sysEnvConfigPtr;
						return true;
					}
				}
			}

			/// failed to retrieve port, set it to default
			server_config.m_strWritePort = "9998";
			delete sysEnvConfigPtr;
			return true;

		} 
		catch (...)
		{
			cerr << "SW_LOGGER ERROR"
				<< "Mcsf::ParseServerConfig Failed!"
				<< endl;

			return false;
		}
	}

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
							const string &strElemName,
							const string &strNewElemValue)
	{
		try 
		{
			scoped_ptr<CLogXmlParse> pServerConfigParser(new CLogXmlParse);
			if (pServerConfigParser->LoadConfigXmlFile(strConfigFile)) 
			{
				if (pServerConfigParser->WriteContentByXmlTag(strElemName, strNewElemValue)) 
				{
					return true;
				}
				return false;
			}

			return false;
		} 
		catch (...)
		{
			cerr << "SW_LOGGER ERROR"
				<< "Mcsf::ModifyServerConfig Failed"
				<< endl;

			return false;
		}
	
	}

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
	bool GetLogHeaderInfoFromXml(const std::string &strConfigFile, char* pElemName, std::string &strElemContent)
	{
        try
        {
            bool bRetVal = false;
            scoped_ptr<CLogXmlParse> pLogHeaderInfoParser(new CLogXmlParse());
            if(pLogHeaderInfoParser && pLogHeaderInfoParser->LoadConfigXmlFile(strConfigFile))
            {
                strElemContent = "";
                bRetVal = pLogHeaderInfoParser->GetContentByXmlPath(pElemName, strElemContent);
            }
            return bRetVal;
        }
        catch(...)
        {
            return false;
        }
	}

    /////////////////////////////////////////////////////////////////
    ///	\brief: get xml Field: recursive find from xml FolderPATH: ..\UIH\appdata\version\
    ///	\param:
    ///			[in] const std::string& strXmlDir: 
    ///					the path of xml file
    ///			[in] const std::string& strElemName: 
    ///					the path of xml node
    ///         [out] const std::string &strElemContents: 
    ///                 splice node value of the all xml files in strXmlFolderPath
    ///	\return: true for success, while false for failure.
    /////////////////////////////////////////////////////////////////
    bool GetLogHeaderInfoFromXmlDir(const path& strXmlDir, const std::string& strElemName, std::string& strElemContents)
    {
        try
        {
            if (!fs::exists(strXmlDir) || !fs::is_directory(strXmlDir))
            {
                return false;
            }

            scoped_ptr<CLogXmlParse> pLogHeaderInfoParser(new CLogXmlParse());
            if (!pLogHeaderInfoParser)
            {
                return false;
            }

            bool bRetVal = false;
            string strTempContent = "";
            std::string filePath = "";
            fs::path fileName(filePath);
            fs::recursive_directory_iterator end;
            for (fs::recursive_directory_iterator pos(strXmlDir); pos != end; ++pos)
            { 
                if (fs::is_directory(*pos))
                {
                    continue;
                }
                
                //skip files without ".xml" extension
                if (!boost::iequals(pos->path().extension().string(), ".xml"))
                {
                    continue;
                }

                //Load xml file full path 
                filePath = (*pos).path().string();
                if (!(pLogHeaderInfoParser->LoadConfigXmlFile(filePath)))
                {
                    continue;
                }

                //get context of xml node 
                strTempContent = "";
                if (!(pLogHeaderInfoParser->GetContentByXmlPath(strElemName, strTempContent)))
                {
                    continue;
                }
                if (strTempContent.empty())
                {
                    continue;
                }

                //splicing context
                fileName=filePath;        
                strTempContent = fileName.stem().string() + ": " + strTempContent;
                strElemContents += strTempContent + ", ";
            }

             //remove ", " in right side
            if (strElemContents.size() > 2)
            {
                strElemContents = strElemContents.substr(0, strElemContents.size() - 2);
                bRetVal = true;
            }
            return bRetVal;
        }
        catch(...)
        {
            return false;
        }
    }
} // namespace Mcsf
