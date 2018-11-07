// McsfLogServer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "mcsf_log_socket_receiver.h"
#include "mcsf_log_file.h"
#include "mcsf_log_server_do_parsing.h"
#include "mcsf_isystemenvironment_config.h"
#include "mcsf_systemenvironment_factory.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <signal.h>
#ifdef OPEN_LOGVIEWER_INTERFACE
#include <mcsf_logviewer_interface.h>
#endif

#pragma warning(disable:4125)

using namespace Mcsf::Log;
#ifdef OPEN_LOGVIEWER_INTERFACE
using namespace Mcsf::LogViewerInterface;
#endif
//#ifdef MCSF_PLATFORM_WIN32
//	#ifdef _WIN64
//		static const std::string CONFIG_FILE_NAME = "../appdata/sysconfig/LoggerServerConfiguration.xml";
//	#else
//		static const std::string CONFIG_FILE_NAME = "../../appdata/sysconfig/LoggerServerConfiguration.xml";
//	#endif
//#else
//	static const std::string CONFIG_FILE_NAME = "../appdata/sysconfig/LoggerServerConfiguration.xml";
//#endif

#define COMPUTER_NAME_MAX_LENGTH 16

static const std::string CONFIG_FILE_NAME = "LoggerServerConfiguration.xml";
static const std::string LOG_HEADER_CONFIG_FILE_NAME = "McsfLogHeaderInfo.xml";
static const std::string LOG_PRODUCT_AND_SERIAL_NO = "../service/config/ServiceConfiguration/ServiceSite.xml";
static const std::string LOG_SOFTWARE_VERSION = "../version";
static const char cszEnd[3] = "\003\n";

std::string McsfGetTimeZoneInfo();
void RecordLogServerStartAndQuitStatus(const char* pStatus);


#ifdef MCSF_PLATFORM_WIN32
static HANDLE gs_pMutex = nullptr;
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    gs_pMutex = CreateMutex(NULL, FALSE, "McsfLogServer");
    if (nullptr == gs_pMutex || ERROR_ALREADY_EXISTS == GetLastError())
    {
        std::cout << "A log server instance is already running" << std::endl;
        CloseHandle(gs_pMutex);
        gs_pMutex = nullptr;
        exit(ERROR_ALREADY_EXISTS);
    }
    else
    {
        std::cout << "start a log server instance." << std::endl;
    }

    Mcsf::ServerConfig server_config;

    server_config.m_strWritePort = argc > 1 ? argv[1] : "9998";
    server_config.m_strReadPort = "9999";
    server_config.m_bShowLog = "off";
    server_config.m_bSaveToErrorFile = "off";
    server_config.m_uLogFileSize = "10";
    
    try
    {
        SocketReceiver server("0.0.0.0", server_config.m_strWritePort);
        server.Run();

    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

//     if(!Mcsf::CErrorHandler::Initial())
//     {
//         std::cout << "fail to set error handler in logserver" << std::endl;
//     }

// 	Mcsf::ServerConfig serverconfig;
// 	Mcsf::ISystemEnvironmentConfig *sysEnvConfigPtr = 
// 		Mcsf::ConfigSystemEnvironmentFactory::Instance()->GetSystemEnvironment();
// 	if (sysEnvConfigPtr)
// 	{
// 		std::string configFilePath = sysEnvConfigPtr->GetSystemConfigFilePath();
// 		if (configFilePath.empty())
// 		{
// 			std::cerr << "GetSystemConfigFilePath returns an empty string" << std::endl;
// 			delete sysEnvConfigPtr;
// 			sysEnvConfigPtr = NULL;
// 			return 1;
// 		}
// 		delete sysEnvConfigPtr;
// 		sysEnvConfigPtr = NULL;
// 
//         std::string strTmp(""), strLogHeaderInfo("");
//         char tag_name[][40] = {"Root/Site/System/ProductNo", "Root/Site/System/SerialNo"};
//         Mcsf::GetLogHeaderInfoFromXml(configFilePath + LOG_PRODUCT_AND_SERIAL_NO, tag_name[0], strTmp);
//         strTmp = ", ProductNo: " + strTmp;
//         strLogHeaderInfo = McsfGetSystemInfo() + strTmp;
//         Mcsf::GetLogHeaderInfoFromXml(configFilePath + LOG_PRODUCT_AND_SERIAL_NO, tag_name[1], strTmp);
//         strTmp = ", SerialNo: " + strTmp;
//         strLogHeaderInfo += strTmp;
//            
//         strLogHeaderInfo += ", timezone: ";
//         strLogHeaderInfo += McsfGetTimeZoneInfo();
// 
// 
//         strTmp = "";    //Erase the previous info
//         const std::string strVersion = "Version/ProductInfo/SoftwareFullVersion";
//         Mcsf::GetLogHeaderInfoFromXmlDir(configFilePath + LOG_SOFTWARE_VERSION , strVersion, strTmp);
//         strTmp = ", " + strTmp;
// 
//         strLogHeaderInfo += strTmp;
//         strLogHeaderInfo += cszEnd;
//         LogFile::instance()->SetLogFileHeader(strLogHeaderInfo);
//         RecordLogServerStartAndQuitStatus("LogServer Start Successfully!");
// 		if (Mcsf::ParseServerConfig(configFilePath + CONFIG_FILE_NAME, serverconfig))
// 		{	
//             LogFile::instance()->setLogFileMaxSize(boost::lexical_cast<std::size_t>(serverconfig.m_uLogFileSize) * 1048576u);
// 			SocketReceiver server("0.0.0.0", serverconfig.m_strWritePort);
// 			boost::thread server_thread(
// 				boost::bind(&SocketReceiver::Run, &server));
// 			while(true)
// 			{
// 				std::string cmd ;
// 				std::cin>>cmd;
// 				if(cmd == "stop")
// 				{
// 					server.Stop();
// 					server_thread.join();
// 					break;
// 				}
// 				//if(cmd == "...")
// 			}
// 
// 		}
// 	}
// 
// 	std::cin.get();
// 
// #ifdef MCSF_PLATFORM_WIN32
//     //just only one instance of log server is allowed to run.
//     {
//         if (nullptr != gs_pMutex)
//         {
//             CloseHandle(gs_pMutex);
//             gs_pMutex = nullptr;
//         }
//     }
// #endif
//     RecordLogServerStartAndQuitStatus("LogServer exit!");
	return 0;
}

std::string McsfGetOSInfo()
{
    //get system name
    std::string strOsName("");
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    OSVERSIONINFOEXA os;
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
    if(GetVersionExA((OSVERSIONINFOA *)&os))
    {
        //decide os name by version info.
        switch(os.dwMajorVersion)
        {
        case 4:
            {
                switch(os.dwMinorVersion)
                {
                case 0:
                    {
                        if(os.dwPlatformId == VER_PLATFORM_WIN32_NT)
                        {
                            strOsName = "Microsoft Windows NT 4.0";
                        }
                        else if(os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
                        {
                            strOsName = "Microsoft Windows 95";
                        }
                        break;
                    }
                case 10:
                    {
                        strOsName = "Microsoft Windows 98";
                        break;
                    }
                case 90:
                    {
                        strOsName = "Microsoft Windows Me";
                        break;
                    }
                default:
                    {
                        strOsName = "unknown os";
                        break;
                    }
                }
                break;
            }
        case 5:
            {
                switch(os.dwMinorVersion)
                {
                case 0:
                    {
                        strOsName = "Microsoft Windows 2000";
                        break;
                    }
                case 1:
                    {
                        strOsName = "Microsoft Windows XP";
                        break;
                    }
                case 2:
                    {
                        if(os.wProductType == VER_NT_WORKSTATION &&
                            info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
                        {
                            strOsName = "Microsoft Windows XP Professional X64 Edition";
                        }
                        else if(0 == GetSystemMetrics(SM_SERVERR2))
                        {
                            strOsName = "Microsoft Windows Server 2003";
                        }
                        else
                        {
                            strOsName = "Microsoft Windows Server 2003 R2";
                        }
                        break;
                    }
                default:
                    {
                        strOsName = "unknown os";
                        break;
                    }
                }
                break;
            }
        case 6:
            {
                switch(os.dwMinorVersion)
                {
                case 0:
                    {
                        if(os.wProductType == VER_NT_WORKSTATION)
                        {
                            strOsName = "Microsoft Windows Vista";
                        }
                        else
                        {
                            strOsName = "Microsoft Windows Server 2008";
                        }
                        break;
                    }
                case 1:
                    {
                        if(os.wProductType == VER_NT_WORKSTATION)
                        {
                            strOsName = "Microsoft Windows 7";
                        }
                        else
                        {
                            strOsName = "Microsoft Windows Server 2008 R2";
                        }
                        break;
                    }
                default:
                    {
                        strOsName = "unknown os";
                        break;
                    }
                }
                break;
            }
        default:
            {
                strOsName = "unknown os!";
                break;
            }
        }
        char szVerInfo[5];
        itoa(os.dwMajorVersion, szVerInfo, 10);
        strOsName += " ";
        strOsName += szVerInfo;
        strOsName += ".";
        itoa(os.dwMinorVersion, szVerInfo, 10);
        strOsName += szVerInfo;
        strOsName += " build ";
        itoa(os.dwBuildNumber, szVerInfo, 10);
        strOsName += szVerInfo;
        strOsName += " Service Pack ";
        itoa(os.wServicePackMajor, szVerInfo, 10);
        strOsName += szVerInfo;
        strOsName += ".";
        itoa(os.wServicePackMinor, szVerInfo, 10);
        strOsName += szVerInfo;
    }
    return strOsName;
}

std::string McsfGetSystemInfo()
{
    std::string strRetVal("");
    DWORD iSize = COMPUTER_NAME_MAX_LENGTH + 1;
    char cCmpName[COMPUTER_NAME_MAX_LENGTH + 1];
    memset(cCmpName, 0, sizeof(cCmpName));
    strRetVal += "hostID:";
    GetComputerNameA(cCmpName, &iSize);
    strRetVal += cCmpName;
    strRetVal += ",";
    strRetVal += "OS:";
    strRetVal += McsfGetOSInfo();
    return strRetVal;
}

std::string McsfGetTimeZoneInfo()
{
    TIME_ZONE_INFORMATION tzi;
    std::string strStandName("unknown timezone");
	DWORD dwRet = GetTimeZoneInformation(&tzi);
    int iSize = WideCharToMultiByte(CP_UTF8, 0, tzi.StandardName, -1, NULL, 0, NULL, NULL);
    if(iSize < 0)
    {
        return strStandName;
    }
    char *pZoneName = new (std::nothrow)char[iSize + 1];
    if(pZoneName)
    {
        memset(pZoneName, 0, iSize + 1);
        WideCharToMultiByte(CP_UTF8, 0, tzi.StandardName, -1, pZoneName, iSize + 1, NULL, NULL);
        strStandName = pZoneName;
        delete []pZoneName;
        pZoneName = nullptr;
    }
	long bias = 0;
    if (TIME_ZONE_ID_DAYLIGHT == dwRet )
    {
		bias = tzi.Bias + tzi.DaylightBias;
    } 
    else
    {
		bias = tzi.Bias;
    }
	char timeZone[128];
	if (bias < 0)
	{
		sprintf (timeZone, "(UTC+%02d:%02d) %s",abs(bias)/60, abs(bias)%60, strStandName.c_str());
	} 
	else
	{
		sprintf (timeZone, "(UTC-%02d:%02d) %s",bias/60, bias%60, strStandName.c_str());
	}
    return std::string(timeZone);
}

void RecordLogServerStartAndQuitStatus(const char* pStatus)
{
    boost::shared_array<char> caLogOnServer(new char[1024]);
    std::string strLog("");
    //std::size_t pos = 0, length = strlen("LOG_DEV_INFO\002");
    //memcpy(caLogOnServer.get(), "LOG_DEV_INFO\002", length);
    std::size_t length = 0;
    strLog = "LOG_DEV_INFO\002";
    char cTime[22];
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);	
    union {
        FILETIME as_file_time;
        boost::uint64_t as_integer; // 100-nanos since 1601-Jan-01
    } time_caster;
    time_caster.as_file_time = ft;
    time_caster.as_integer -= 116444736000000000ULL; // filetime is now 100-nanos since 1970-Jan-01
    sprintf(cTime, "%"UINT64_FMT"\002", time_caster.as_integer / 10000UL);
    strLog += cTime;
    strLog += "LogServer(";
    strLog += boost::lexical_cast<std::string>(GetCurrentProcessId()) + ":" +
        boost::lexical_cast<std::string>(GetCurrentThreadId()) + ")\002";
    char caFile[256];
    strcpy(caFile, __FILE__);
    length = strlen(caFile);
    while(length)
    {
        length --;
        if(caFile[length] == '/' || caFile[length] == '\\')
        {
            length += 1;
            break;
        }
    }
    strLog += &caFile[length];
    char strLineNo[8];
    sprintf(strLineNo, "\002%d\002", __LINE__);
    strLog += strLineNo;    
    strLog += __FUNCTION__;
    strLog += "\0020X0000000000000000\002";
    strLog += pStatus;
// 	strLog+="\002";
// 	strLog += cTime;
    strLog += "\001\n";
    length = strLog.length();
    memcpy(caLogOnServer.get(), strLog.c_str(), length);
	uint64_t deltaTime = 0;
    LogFile::instance()->Write(caLogOnServer, length, deltaTime);
}