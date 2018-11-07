#include "../stdafx.h"

#include "mcsf_logger_base.h"
#include "mcsf_log_processor.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

using namespace Mcsf::Log;
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
#include <linux/unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#elif defined(_WIN32)
#include <Windows.h>
#endif

#if defined(MCSF_PLATFORM_WIN32)
#pragma warning(disable:4127)
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4251)
#pragma warning(disable:4996)
#endif

#include <time.h>
#include <iomanip>
#include <algorithm>
#include <boost/thread/tss.hpp> 
#include <boost/thread/xtime.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> 

namespace Mcsf {

    const std::size_t			LOG_MAX_LENGTH = 1024;
    const std::size_t			LOG_CONTENT_MAX_LENGTH = 512;
    const std::size_t           AUDIT_LOG_MAX_LENGTH = 2560;
	const std::size_t           AUDIT_CONTENT_MAX_LENGTH = 2048;
    static const int			CONST_LOGUID_ZERO = 0x0;
    static const std::string	LOGGER_SOURCE_NAME = "DefaultLoggerSource";
    static std::string			g_sClientName = "";
    static std::string          g_sMachineName = "";
    static int					g_iLogProcessID = 0;
    static int					g_iLogThreadID = 0;
    static bool					g_bLoggerInstanceFlag = false;
    static bool					g_bLogTraceInfo = false;
    static bool					g_bLogTraceWarning = false;
    static bool					g_bLogTraceError = false;
    static bool                 g_bPerformaceRecord = false;
    static bool                 g_bAuditLogInfo = false;

    static bool					g_bLogTraceInfo_default = false;
    static bool					g_bLogTraceWarning_default = false;
    static bool					g_bLogTraceError_default = false;

    static bool					g_bLogTraceSwitch = true;

    static bool					g_bAuditLogSwitch = false;

    static bool					g_bEnable_Send = true;

    //static const int			LOG_TRACE_NONE_OUTPUT   = 0;
    //static const int			LOG_TRACE_ERROR_CHANGED = 1;
    //static const int			LOG_TRACE_WARN_CHANGED  = 2;
    //static const int			LOG_TRACE_INFO_CHANGED  = 3;
    static const std::size_t	LOG_LEN_LEN = 2;

    static ProcessorNodePtr	g_pRootNode = ProcessorNodePtr();
    static boost::thread_specific_ptr<std::string>     g_PtrTlsSource;
    static boost::mutex                                g_mutex;
    static bool g_bLogInitialized = false;
    static bool g_bGetHostName = false;

    static const std::size_t    MAX_HOSTNAME_LENGTH = 256;
    static const std::size_t    MAX_SYSLOG_HEADER_LENGTH = 512;

#ifdef MCSF_PLATFORM_WIN32
    static LARGE_INTEGER g_performanceFrequency;
#endif
#if defined MCSF_PLATFORM_WIN32
#define UINT64_FMT "I64u"
#elif defined MCSF_PLATFORM_POSIX
#define UINT64_FMT "llu"
#endif

#define EXT_C_DLLEXPORT extern "C" _declspec(dllexport)

#define INIT_LEVEL_TIME_SOURCE(sLogLevel, stime, sSource, ppTmp) \
	{ \
	*ppTmp = logcpy(*ppTmp, sLogLevel); \
	if(stime) \
	{ \
	*ppTmp = logcpy(*ppTmp, stime); \
	} \
	else \
	{ \
	boost::uint64_t log_time = get_log_time(); \
	char acLogTime[21] = {0}; \
	itoa_on_rtos(log_time, acLogTime, 10); \
	*ppTmp = logcpy(*ppTmp, acLogTime); \
	} \
	\
	*ppTmp = logcpy(*ppTmp, GLogGetSource(sSource, _log_type).c_str()); \
	_length = *ppTmp - _log; \
	}

#define INIT_FILE(pfile, ppTmp, bResult) \
	{ \
	std::size_t tmplen = strlen(pfile); \
	int iBackslashPos = static_cast<int>(tmplen - 1); \
	while(iBackslashPos >= 0) \
	{ \
	if('\\' == pfile[iBackslashPos] || '/' == pfile[iBackslashPos] || '/' == pfile[iBackslashPos]) \
	{ \
	break; \
	} \
	iBackslashPos--; \
	} \
	\
	if(iBackslashPos >= 0) \
	{ \
	tmplen = tmplen - iBackslashPos - 1; \
	pfile = &pfile[iBackslashPos + 1]; \
	} \
	\
	if(tmplen > _log_max_length - _length) \
	{ \
	*bResult = false; \
	} \
	else \
	{ \
	*ppTmp = logcpy(*ppTmp, pfile); \
	*bResult = true; \
	} \
	}

#define INIT_REMAINING(pfun, lineNum, ulLogUid, ppTmp, bResult) \
	{ \
	char acLineNum[8] = {0}; \
	itoa_on_rtos(lineNum, acLineNum, 10); \
	if(strlen(acLineNum) + _length > _log_max_length) \
	{ \
	*bResult = false; \
	} \
	else \
	{ \
	*ppTmp = logcpy(*ppTmp, acLineNum); \
	_length = *ppTmp - _log; \
	\
	if(strlen(pfun) + _length > _log_max_length) \
	{ \
	*bResult = false; \
	} \
		else \
	{ \
	*ppTmp = logcpy(*ppTmp, pfun); \
	_length = *ppTmp - _log; \
	\
	char acLogid[19] = {0}; \
	acLogid[0] = '0'; \
	acLogid[1] = 'X'; \
	itoa_on_rtos(ulLogUid, &acLogid[2], 16, 16); \
	\
	if(strlen(acLogid) + _length > _log_max_length) \
	{ \
	*bResult = false; \
	} \
			else \
	{ \
	*ppTmp = logcpy(*ppTmp, acLogid); \
	_length = *ppTmp - _log; \
	*bResult = true; \
	} \
	} \
	} \
	}

#define CLOG_ACTUAL_CONSTRUCTOR(sSource, ulLogUid, pfile, pfun, lineNum, sLogLevel, stime) \
	{ \
	try \
	{ \
	if(_enable && NULL != g_pRootNode) \
	{ \
	do { \
	bool bResult = false;\
	char *tmpLog = _log = new char[_log_max_length]; \
	char **pTmp = &tmpLog; \
	INIT_LEVEL_TIME_SOURCE(sLogLevel, stime, sSource, pTmp) \
	INIT_FILE(pfile, pTmp, (&bResult)) \
	if(!bResult) \
	break;\
	INIT_REMAINING(pfun, lineNum, ulLogUid, pTmp, (&bResult)) \
	if(!bResult) \
	break;  \
	} while(0); \
	} \
	} \
	catch(...) \
	{ \
	delete []_log; \
	_log = NULL; \
	} \
	}

    static char *itoa_on_rtos(LogTypes::int64_t iNum, char *acString, int iRadix, int format_length = 0)
    {
        return LogHelper::itoa_on_rtos(iNum, acString, iRadix, format_length);
    }

    static char * logcpy(char * dst, const char * src)
    {
        char * cp = dst;
        while ('\0' != (*cp++ = *src++));
        *(cp - 1) = '\002';
        return(cp);
    }

    static boost::uint64_t get_log_time()
    {
        return SocketSender::GetLogTimeU64();
    }

    // Get Pid
    static void inline GetPid()
    {
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
        g_iLogProcessID = getpid();
#elif defined(_WIN32) 
        g_iLogProcessID = GetCurrentProcessId();
#endif
    }

    // Get Tid
    static void inline GetTid()
    {
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
        g_iLogThreadID = (int)syscall(SYS_gettid);
#elif defined(_WIN32)
        g_iLogThreadID = GetCurrentThreadId();
#endif
    }

    static void MapDomainName2Ip(std::string& strDomainName)
    {
        namespace ns = boost::asio::ip;
        boost::system::error_code error = boost::asio::error::host_not_found;
        boost::asio::io_service ioService;
        ns::tcp::resolver resolver(ioService);
        ns::tcp::resolver::query query(ns::tcp::v4(), strDomainName, "0");
        ns::tcp::resolver::iterator end;
        ns::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query, error);
        if (!error)
        {
            while (endpoint_iterator != end)
            {
                strDomainName = (*endpoint_iterator++).endpoint().address().to_string();
                if (0 == strDomainName.compare("127.0.0.1"))
                    continue;
                break;
            }
        }
    }

#ifdef WIN32
    EXT_C_DLLEXPORT void MapDomainName2IpForCSharp(char **ppDomainName)
    {
        std::string strDomainName = *ppDomainName;
        MapDomainName2Ip(strDomainName);
        strcpy(*ppDomainName, strDomainName.c_str());
    }
#endif
    ConfigInfo g_config;
    // Get Logger Instance and Init Global Parameter
    void GLogLoadConfig(const std::string& sConfigXMLFile)
    {
#ifdef MCSF_PLATFORM_WIN32
        if (!QueryPerformanceFrequency(&g_performanceFrequency))
        {
            g_performanceFrequency.QuadPart = 0;
        }
#endif
        if (g_bLogInitialized)
            return;
        try
        {
            GetPid();

//             TraceSwitch traceSwitch;
//             ParsingTraceSwitchConfig(traceSwitch);
//             boost::to_lower(traceSwitch.m_traceSwitch);
//             g_bLogTraceSwitch = (traceSwitch.m_traceSwitch == "on") ? true : false;
//             boost::to_lower(traceSwitch.m_performanceRecord);
//             g_bPerformaceRecord = (traceSwitch.m_performanceRecord == "on") ? true : false;
//             boost::to_lower(traceSwitch.m_auditLogSwitch);
//             g_bAuditLogSwitch = (traceSwitch.m_auditLogSwitch == "on") ? true : false;


           
            if (ParsingServerConfig(sConfigXMLFile, g_config))
            {
                MapDomainName2Ip(g_config.m_sServerIP);
                boost::to_lower(g_config.m_sLogTraceError);
                boost::to_lower(g_config.m_sLogTraceInfo);
                boost::to_lower(g_config.m_sLogTraceWarn);
                bool bLogTraceInfo = (g_config.m_sLogTraceInfo == "on") ? true : false;
                g_bLogTraceInfo = g_bLogTraceSwitch && bLogTraceInfo;
                bool bLogTraceWarning = (g_config.m_sLogTraceWarn == "on") ? true : false;
                g_bLogTraceWarning = g_bLogTraceSwitch && bLogTraceWarning;
                bool bLogTraceError = (g_config.m_sLogTraceError == "on") ? true : false;
                g_bLogTraceError = g_bLogTraceSwitch && bLogTraceError;
                g_bLogTraceInfo_default = g_bLogTraceInfo;
                g_bLogTraceWarning_default = g_bLogTraceWarning;
                g_bLogTraceError_default = g_bLogTraceError;
                if (g_config.m_sClientName.empty())
                {
                    g_sClientName = "NA";
                }
                else
                {
                    g_sClientName = g_config.m_sClientName;
                }
                *ProcessorNode::GetClientName() = g_sClientName;

                g_bLoggerInstanceFlag = true;
                g_pRootNode = ProcessorNodePtr(new ProcessorNode(ProcessorPtr(new LogBuffer()), LOG_Q_DEFAULT_MAX_SIZE, sg_carrThreshhold[0], true));
                ProcessorNodePtr socket_node
                    = ProcessorNodePtr(new ProcessorNode(ProcessorPtr(new SocketSender(g_config.m_sServerIP
                        , g_config.m_sServerPort)), 1, sg_carrThreshhold[1], false));
                g_pRootNode->SetChildNode(socket_node);
                g_pRootNode->start();
                g_bEnable_Send = true;
                g_bLogInitialized = true;
            }
        }
        catch (...)
        {
            std::cerr << "SW_LOGGER_ERROR: Exception on GLogLoadConfig" << std::endl;
        }
    }

    class LogEnterLeave
    {
    public:
        LogEnterLeave(const char* pfile, const char* pfun, int lineNum)
            : _filename(pfile), _funname(pfun), _linenum(lineNum)
        {
            CLogTraceInfo("LogClient", 0, pfile, pfun, lineNum, "LOG_TRACE_INFO") << "enter.";
        }

        ~LogEnterLeave()
        {
            CLogTraceInfo("LogClient", 0, _filename, _funname, _linenum, "LOG_TRACE_INFO") << "leave.";
        }

    private:
        const char*		_filename;
        int				_linenum;
        const char*		_funname;
    };
    ///brief: check if local buffer file exist
    ///param: VOID
    ///retval: true for existence, while false for non existence.
    inline static bool IsLocalLogsExist()
    {
        //  LogEnterLeave x(__FILE__, __FUNCTION__, __LINE__);
        try
        {
            namespace nsBF = boost::filesystem;
            nsBF::path pthLog = nsBF::path(g_sLocalLogPath);
            pthLog = nsBF::system_complete(pthLog);
            if (!nsBF::exists(pthLog) || !nsBF::is_directory(pthLog))
            {
                return false;
            }
            nsBF::directory_iterator dirIter(pthLog), dirEnd;
            //	boost::regex e(".+\\.uihlog$");
            for (; dirIter != dirEnd; ++dirIter)
            {
                if (nsBF::is_regular_file(*dirIter) 
                    && dirIter->path().extension().string() == ".uihlog" 
                    && !boost::contains(dirIter->path().stem().string(), "McsfRunJob")) 
                {
                    return true;
                }
            }
            return false;
        }
        catch (boost::filesystem::filesystem_error &e)
        {
            CLog("LogClient", 0, __FILE__, __FUNCTION__, __LINE__, "LOG_DEV_ERROR") << e.what();
            return false;
        }
        catch (...)
        {
            CLog("LogClient", 0, __FILE__, __FUNCTION__, __LINE__, "LOG_DEV_ERROR") << "unknown exception!";
            return false;
        }
    }

    ///brief: check if log server is available
    ///param: VOID
    ///retval: true for available, while false for unavailable.
    inline static bool IsLogServerAvailable()
    {
        //  LogEnterLeave x(__FILE__, __FUNCTION__, __LINE__);
        try
        {
            //just get the connected status from client
            if (g_pRootNode)
            {
                IProcessor *pProcessor = g_pRootNode->GetChildNode()->get_processor().get();
                if (NULL == pProcessor)
                {
                    return false;
                }

                SocketSender *pSend = dynamic_cast<SocketSender*>(pProcessor);
                if (NULL == pSend)
                {
                    return false;
                }

                return pSend->get_connect_status();
            }
            else
            {
//                 ConfigInfo config;
//                 ParsingServerConfig()
             //   GetLogServerIpAndPort(config);
                namespace addrns = boost::asio::ip;
                namespace tcpns = boost::asio::ip;
                addrns::address addr = addrns::address::from_string(g_config.m_sServerIP);
                tcpns::tcp::endpoint endpoint(addr, boost::lexical_cast<unsigned short>(g_config.m_sServerPort));
                boost::asio::io_service _service;
                boost::asio::ip::tcp::socket _socket(_service);
                boost::system::error_code err;
                _socket.connect(endpoint, err);
                if (err)
                {
                    _socket.close();
                    return false;
                }
            }
            return true;
        }
        catch (...)
        {
            CLog("LogClient", 0, __FILE__, __FUNCTION__, __LINE__, "LOG_DEV_ERROR") << "unknown exception!";
            return false;
        }
    }

    bool GLogIsLocalTransferTobeLaunched()
    {
        //check if local files exist
        return IsLocalLogsExist() && IsLogServerAvailable();
    }

    void GSetLogTraceInfo(bool on)
    {
        g_bLogTraceInfo = on;
    }

    void GSetLogTraceWarning(bool on)
    {
        g_bLogTraceWarning = on;
    }

    void GSetLogTraceError(bool on)
    {
        g_bLogTraceError = on;
    }

    void GLogLoadConfig(const char* sConfigXMLFile)
    {
        std::string filename(sConfigXMLFile);
        GLogLoadConfig(filename);
    }

    bool GLogUnloadConfig()
    {
        g_bLogInitialized = false;
        g_bEnable_Send = false;
        if (NULL != g_pRootNode)
            g_pRootNode->stop();
        return true;
    }

    bool GLogFlush()
    {
//         if (NULL != g_pRootNode)
//             g_pRootNode->flush();
        return true;
    }

    void GLogSetSource(const std::string& sSource)
    {
        try
        {
            boost::mutex::scoped_lock lock_1(g_mutex);
            std::string sTempSource = boost::trim_copy(sSource);
            GetTid();
            if (sTempSource.empty() && g_sClientName.empty())
            {
                sTempSource = LOGGER_SOURCE_NAME;
            }
            else if ((sTempSource.empty()) && !g_sClientName.empty())
            {
                sTempSource = g_sClientName;
            }

            *ProcessorNode::GetClientName() = sTempSource;

            if (g_sMachineName != "")
            {
                sTempSource = sTempSource + "("
                    + boost::lexical_cast<std::string>(g_sMachineName)
                    + ":" + boost::lexical_cast<std::string>(g_iLogProcessID)
                    + ":" + boost::lexical_cast<std::string>(g_iLogThreadID) + ")";
            }
            else
            {
                sTempSource = sTempSource + "("
                    + boost::lexical_cast<std::string>(g_iLogProcessID)
                    + ":" + boost::lexical_cast<std::string>(g_iLogThreadID) + ")";
            }

            g_PtrTlsSource.reset(new std::string(sTempSource));
        }
        catch (...)
        {
            std::cout << "SW_LOGGER_ERROR: Exception on GLogSetSource" << std::endl;
        }
    }

    void GLogSetSource(const char* sSource)
    {
        std::string source(sSource);
        GLogSetSource(source);
    }

    std::string GLogGetSource(const std::string& sSource, LogType type)
    {
        try
        {
            std::string sTlsSource = "";
            std::string sTempSource = sSource;
            if (sTempSource.empty())                    // Condition 1
            {
                sTempSource = LOGGER_SOURCE_NAME;
            }
            if (sTempSource == LOGGER_SOURCE_NAME
                && NULL != g_PtrTlsSource.get())    // Condition 2 
            {
                sTlsSource = *g_PtrTlsSource;
            }
            else if (sTempSource == LOGGER_SOURCE_NAME
                && NULL == g_PtrTlsSource.get())
            {
                if (!g_sClientName.empty())
                {
                    sTempSource = g_sClientName;
                }
                GLogSetSource(sTempSource);
                sTlsSource = *g_PtrTlsSource;
            }
            else if (sTempSource != LOGGER_SOURCE_NAME) // Condition 3
            {
                GetTid();

                if (AUDIT_LOG == type)
                {
                    static char audit_host_name[MAX_HOSTNAME_LENGTH];
                    if (!g_bGetHostName)
                    {
                        if (-1 == gethostname(audit_host_name, sizeof(audit_host_name)))
                        {
                            std::cerr << "get host name error!" << std::endl;
                        }
                        g_bGetHostName = true;
                    }
                    sTempSource = sTempSource + "("
                        + boost::lexical_cast<std::string>(audit_host_name)
                        + ":" + boost::lexical_cast<std::string>(g_iLogProcessID)
                        + ":" + boost::lexical_cast<std::string>(g_iLogThreadID) + ")";
                }
                else
                {
                    if (g_sMachineName != "")
                    {
                        sTempSource = sTempSource + "("
                            + boost::lexical_cast<std::string>(g_sMachineName)
                            + ":" + boost::lexical_cast<std::string>(g_iLogProcessID)
                            + ":" + boost::lexical_cast<std::string>(g_iLogThreadID) + ")";
                    }
                    else
                    {
                        sTempSource = sTempSource + "("
                            + boost::lexical_cast<std::string>(g_iLogProcessID)
                            + ":" + boost::lexical_cast<std::string>(g_iLogThreadID) + ")";

                    }
                }
                sTlsSource = sTempSource;
            }
            return sTlsSource;
        }
        catch (...)
        {
            std::cout << "SW_LOGGER_ERROR: Exception on GLogGetSource" << std::endl;
            return "";
        }
    }

    //field: 1bit +  1bit + 1bit
    //          |       |      |
    //iLevel: error + warn + info
    bool GSetTraceLevel(int iLevel)
    {
        boost::mutex::scoped_lock lock_1(g_mutex);
        g_bLogTraceError = (iLevel & 0x04) == 0x04;
        g_bLogTraceWarning = (iLevel & 0x02) == 0x02;
        g_bLogTraceInfo = (iLevel & 0x01) == 0x01;
        return true;
    }

    void GResetTraceLevel()
    {
        g_bLogTraceInfo = g_bLogTraceInfo_default;
        g_bLogTraceWarning = g_bLogTraceWarning_default;
        g_bLogTraceError = g_bLogTraceError_default;
    }


    int GGetTraceLevel()
    {
        return (g_bLogTraceError << 2) + (g_bLogTraceWarning << 1) + g_bLogTraceInfo;
    }

    bool    MCSF_LOGGER_API       GIsTraceInfoOn()
    {
        return g_bLogTraceInfo;
    }
    bool    MCSF_LOGGER_API       GIsTraceWarningOn()
    {
        return g_bLogTraceWarning;
    }
    bool    MCSF_LOGGER_API       GIsTraceErrorOn()
    {
        return g_bLogTraceError;
    }

    bool    MCSF_LOGGER_API       GIsPerformanceRecordOn()
    {
        return g_bPerformaceRecord;
    }

    void GSetOutputFlag(bool bOutputFlag)
    {
        g_bEnable_Send = bOutputFlag;
        //TODO send log to console
    }

    CLog::CLog(const std::string& sSource, const LogTypes::uint64_t ulLogUid,
        const char* pfile, const char* pfun, int lineNum, const char* level, bool enable, LogType type)
        : _enable(enable), _log(NULL), _length(0/*LOG_LEN_LEN*/), _log_max_length(type == NORMAL_LOG ? LOG_MAX_LENGTH : AUDIT_LOG_MAX_LENGTH),
		  _log_content_max_length(type == NORMAL_LOG ? LOG_CONTENT_MAX_LENGTH : AUDIT_CONTENT_MAX_LENGTH),_log_type(type)
    {
        CLOG_ACTUAL_CONSTRUCTOR(sSource.c_str(), ulLogUid, pfile, pfun, lineNum, level, NULL)
    }

    CLog::CLog(const char* sSource, const LogTypes::uint64_t ulLogUid,
        const char* pfile, const char* pfun, int lineNum, const char* level, bool enable, LogType type)
        : _enable(enable), _log(NULL), _length(0/*LOG_LEN_LEN*/), _log_max_length(type == NORMAL_LOG ? LOG_MAX_LENGTH : AUDIT_LOG_MAX_LENGTH),
		  _log_content_max_length(type == NORMAL_LOG ? LOG_CONTENT_MAX_LENGTH : AUDIT_CONTENT_MAX_LENGTH),_log_type(type)
    {
        CLOG_ACTUAL_CONSTRUCTOR(sSource, ulLogUid, pfile, pfun, lineNum, level, NULL)
    }

    bool CLog::IsLogSwitchOn(const::string& sLogLevel)
    {
        bool ret = true;
        if (0 == sLogLevel.compare("LOG_TRACE_INFO"))
        {
            ret = g_bLogTraceInfo;
        }
        else if (0 == sLogLevel.compare("LOG_TRACE_WARN"))
        {
            ret = g_bLogTraceWarning;
        }
        else if (0 == sLogLevel.compare("LOG_TRACE_ERROR"))
        {
            ret = g_bLogTraceError;
        }
        else if (0 == sLogLevel.compare("LOG_AUDIT_INFO"))
        {
            ret = g_bAuditLogInfo;
        }
        return ret;
    }

    CLog::CLog(const std::string& sSource, const LogTypes::uint64_t ulLogUid,
        const char* pfile, const char* pfun, int lineNum, const std::string& sLogLevel
        , const std::string& stime)
        :_log(NULL), _length(0/*LOG_LEN_LEN*/), _log_max_length(LOG_MAX_LENGTH), _log_content_max_length(LOG_CONTENT_MAX_LENGTH)
    {
        _enable = IsLogSwitchOn(sLogLevel);
        CLOG_ACTUAL_CONSTRUCTOR(sSource.c_str(), ulLogUid, pfile, pfun, lineNum, sLogLevel.c_str(), (stime.empty()) ? NULL : stime.c_str())
            //CLOG_ACTUAL_CONSTRUCTOR(sSource.c_str(), ulLogUid, pfile, pfun, lineNum, sLogLevel.c_str(), stime.c_str())
    }

    CLog::CLog(const char* sSource, const LogTypes::uint64_t ulLogUid,
        const char* pfile, const char* pfun, int lineNum, const char* sLogLevel
        , const char* stime)
        :_log(NULL), _length(0/*LOG_LEN_LEN*/), _log_max_length(LOG_MAX_LENGTH), _log_content_max_length(LOG_CONTENT_MAX_LENGTH)
    {
        _enable = IsLogSwitchOn(sLogLevel);
        CLOG_ACTUAL_CONSTRUCTOR(sSource, ulLogUid, pfile, pfun, lineNum, sLogLevel, (!stime || '\0' == stime[0]) ? NULL : stime)
            //CLOG_ACTUAL_CONSTRUCTOR(sSource, ulLogUid, pfile, pfun, lineNum,sLogLevel, stime)
    }

    CLog::~CLog()
    {
        send();
    }


    void  GLogWriteToBuffer(const char* log)
    {
        if (g_bEnable_Send && NULL != g_pRootNode && log)
        {
            std::size_t tmplen = strlen(log);
            char *buf = NULL;
            try
            {
                buf = new char[tmplen + 1];
            }
            catch (const bad_alloc&)//内存分配失败
            {
                return;
            }
            try
            {
                memset(buf, 0, tmplen + 1);
                memcpy(buf, log, tmplen);
                LogPtr log_to_be_send = LogPtr(new RawLog());
                log_to_be_send->_data = CharArray(buf);
                log_to_be_send->_length = tmplen;
                try
                {
                    g_pRootNode->DoSink(log_to_be_send, true, false);
                }
                catch (...)
                {
                }
            }
            catch (...)
            {
                //释放内存
                delete[] buf;
                buf = NULL;
            }
        }
    }


    void CLog::send()
    {
        try
        {
            if (_enable && g_bEnable_Send && NULL != g_pRootNode && _log)
            {
                do
                {
#ifdef MCSF_PLATFORM_WIN32
                    static time_t t;
                    static char datagramm[MAX_SYSLOG_HEADER_LENGTH];
                    static char local_hostname[MAX_HOSTNAME_LENGTH];
                    static bool b_get_hostname = false;

                    if (AUDIT_LOG == _log_type)
                    {
                        if (!b_get_hostname)
                        {
                            WSADATA w;
                            WSAStartup(0x0101, &w);
                            int n = sizeof(local_hostname);
                            gethostname(local_hostname, n);
                            WSACleanup();
                        }

                        t = time(0);
                        char tmp[64];
                        strftime(tmp, sizeof(tmp), "%b %d %X", localtime(&t));
                        sprintf(datagramm, "<85> %s %s ", tmp, local_hostname);

                        std::size_t len = strlen(datagramm);
                        memcpy(_log + _length, datagramm, len);
                        _length += len;
                    }
#endif
                    std::string stream_string = _stream.str();
                    std::size_t tmplen = stream_string.size();
                    tmplen = tmplen > _log_content_max_length
                        ? _log_content_max_length : tmplen;
                    if (tmplen > _log_max_length - _length)
                    {
                        break;
                    }
                    memcpy(_log + _length, stream_string.data(), tmplen);
                    _length += tmplen;

                    _log[_length] = LOG_TERMINATOR;//'\001';
                    ++_length;
                    _log[_length] = '\n';
                    ++_length;
                    LogPtr log_to_be_send = LogPtr(new RawLog());
                    log_to_be_send->_data = CharArray(_log);
                    log_to_be_send->_length = _length;
                    g_pRootNode->DoSink(log_to_be_send, true, false);
                } while (0);
            }
        }
        catch (...)
        {
            delete _log;
            _log = NULL;
        }
    }


    CLogTraceError::CLogTraceError(const std::string& sSource, const LogTypes::uint64_t ulLogUid,
        const char* pfile, const char* pfun, int lineNum, const char* level)
        :CLog(sSource, ulLogUid, pfile, pfun, lineNum, level, g_bLogTraceError)
    {

    }


    CLogTraceInfo::CLogTraceInfo(const std::string& sSource, const LogTypes::uint64_t ulLogUid,
        const char* pfile, const char* pfun, int lineNum, const char* level)
        :CLog(sSource, ulLogUid, pfile, pfun, lineNum, level, g_bLogTraceInfo)
    {

    }

    CLogTraceWarning::CLogTraceWarning(const std::string& sSource, const LogTypes::uint64_t ulLogUid,
        const char* pfile, const char* pfun, int lineNum, const char* level)
        : CLog(sSource, ulLogUid, pfile, pfun, lineNum, level, g_bLogTraceWarning)
    {

    }

    CLogAuditLog::CLogAuditLog(const std::string& sSource, const LogTypes::uint64_t ulLogUid,
        const char* pfile, const char* pfun, int lineNum, const char* level)
        : CLog(sSource, ulLogUid, pfile, pfun, lineNum, level, g_bAuditLogSwitch, AUDIT_LOG)
    {
    }

    static inline LogTypes::int64_t get_current_performance_counter()
    {
#ifdef MCSF_PLATFORM_POSIX
        struct timespec ret;
        clock_gettime(CLOCK_MONOTONIC, &ret);
        return ((LogTypes::int64_t)ret.tv_sec) * 1000000000 + ret.tv_nsec;
#elif defined MCSF_PLATFORM_WIN32
        LARGE_INTEGER				ret;
        QueryPerformanceCounter(&ret);
        return ret.QuadPart;
#else
        return 0;
#endif

    }

    CLogExecutingTime::CLogExecutingTime(const char* sSource, const char* pfile, const char* pfun
        , int lineNum, const char* name)
        : _source(sSource), _filename(pfile), _funname(pfun)
        , _linenum(lineNum), _name(name), _trace_on(g_bLogTraceInfo)
    {
        if (!_trace_on)
            return;
        if (sSource == NULL)
            _source = "ExecutingTime";
        CLogTraceInfo(
            _source, CONST_LOGUID_ZERO, pfile, pfun, lineNum, "LOG_TRACE_INFO")
            << "Trace " << _name << " duration start!";
#ifdef MCSF_PLATFORM_POSIX
        m_perf_couter_begin = get_current_performance_counter();
#elif defined MCSF_PLATFORM_WIN32
        if (g_performanceFrequency.QuadPart > 0)
        {
            m_perf_couter_begin = get_current_performance_counter();
        }
#else
        m_perf_couter_begin = 0;
#endif
    }

    CLogExecutingTime::CLogExecutingTime(const char* sSource, const char* pfile
        , const char* pfun, int lineNum)
        : _source(sSource), _filename(pfile), _funname(pfun), _linenum(lineNum), _name("")
        , _trace_on(g_bLogTraceInfo)
    {
        if (!_trace_on)
            return;
        if (sSource == NULL)
            _source = "ExecutingTime";
        CLogTraceInfo(
            _source, CONST_LOGUID_ZERO, pfile, pfun, lineNum, "LOG_TRACE_INFO")
            << "Trace function duration start!";
#ifdef MCSF_PLATFORM_POSIX
        m_perf_couter_begin = get_current_performance_counter();
#elif defined MCSF_PLATFORM_WIN32
        if (g_performanceFrequency.QuadPart > 0)
        {
            m_perf_couter_begin = get_current_performance_counter();
        }
#else
        m_perf_couter_begin = 0;
#endif
    }

    CLogExecutingTime::~CLogExecutingTime()
    {
        try
        {
            if (!_trace_on)
                return;

            LogTypes::int64_t time_elapsed = 0;
#ifdef MCSF_PLATFORM_POSIX		
            LogTypes::int64_t time_end = get_current_performance_counter();
            time_elapsed = time_end - m_perf_couter_begin;
#elif defined MCSF_PLATFORM_WIN32
            if (g_performanceFrequency.QuadPart > 0)
            {
                LogTypes::int64_t time_end = get_current_performance_counter();
                time_elapsed =
                    (time_end - m_perf_couter_begin) * 1000000000
                    / g_performanceFrequency.QuadPart;
            }
#else

#endif
            if (_name == NULL)
                _name = "";
            CLogTraceInfo(
                _source, CONST_LOGUID_ZERO, _filename, _funname, _linenum
                , "LOG_TRACE_INFO")
                << "Trace " << _name << " Executing Time end! time elapsed = "
                << time_elapsed
                << " nanoseconds";
        }
        catch (std::exception& e)
        {
            std::cout << "error in function[" << __FUNCTION__ << "]"
                "with error msg: " << e.what() << std::endl;
        }
    }


    CLogDumpToFile::CLogDumpToFile()
    {
        if (NULL != g_pRootNode)
        {
#if defined(MCSF_PLATFORM_POSIX)
            int processID = getpid();
#elif defined(MCSF_PLATFORM_WIN32)
            int processID = GetCurrentProcessId();
#endif
            char file_name[32];
            sprintf(file_name, "%s_process%d.uihlog", LOG_DUMP_FILE_NAME, processID);
            FileProxyPtr file = FileProxyPtr(new FileProxy(file_name));
            file->Initialize();
            if (file->Ready())
            {
                g_pRootNode->dump(file);
            }
        }
    }

    static char GetBuOfServiceCode(LogTypes::uint8_t iBuOfLogUID)
    {
        switch (iBuOfLogUID)
        {
        case 0://sw
        {
            iBuOfLogUID = 'S';
            break;
        }
        case 1: //xr
        {
            iBuOfLogUID = 'X';
            break;
        }
        case 2: //ct
        {
            iBuOfLogUID = 'C';
            break;
        }
        case 3: //mr
        {
            iBuOfLogUID = 'M';
            break;
        }
        case 4: //mi
        {
            iBuOfLogUID = 'P';
            break;
        }
        case 5: //rt
        {
            iBuOfLogUID = 'R';
            break;
        }
        default:
        {
            iBuOfLogUID = '?';
            break;
        }
        }
        return static_cast<char>(iBuOfLogUID);
    }

    std::string GGetServiceCode(LogTypes::uint64_t iLogUID)
    {
        std::string strServiceCode;
        strServiceCode.reserve(7);
        //BU
        strServiceCode.append(1, GetBuOfServiceCode((iLogUID & 0xf000000000000000) >> 60));
        //physical component
        char caTmpArray[3] = { 0 };
        sprintf(caTmpArray, "%02x", (iLogUID & 0x0000ff0000000000) >> 40);
        strServiceCode.append(caTmpArray);
        //logical component
        sprintf(caTmpArray, "%02x", (iLogUID & 0x000000ff00000000) >> 32);
        strServiceCode.append(caTmpArray);
        //special log code
        sprintf(caTmpArray, "%02x", iLogUID & 0x00000000000000ff);
        strServiceCode.append(caTmpArray);
        return strServiceCode;
    }

    LogTypes::uint64_t GMakeLogUID(LogTypes::uint8_t iBU, LogTypes::uint8_t iModality, LogTypes::uint8_t iErrorLevel,
        LogTypes::uint8_t iPhysicalComponent, LogTypes::uint8_t iLogicalComponent, LogTypes::uint32_t iSpecialCode)
    {
        LogTypes::uint64_t iLogUid = 0x0;
        iLogUid = ((LogTypes::uint64_t)(iBU & 0x0f) << 60) | ((LogTypes::uint64_t)iModality << 52) | ((LogTypes::uint64_t)(iErrorLevel & 0x0f) << 48)\
            | ((LogTypes::uint64_t)iPhysicalComponent << 40) | ((LogTypes::uint64_t)iLogicalComponent << 32) | iSpecialCode;
        return iLogUid;
    }

    sMcsfLogUID GParseLogUID(LogTypes::uint64_t iLogUID)
    {
        sMcsfLogUID mcsfLogUid;
        mcsfLogUid.iBU = (iLogUID >> 60) & 0x0f;
        mcsfLogUid.iModality = (iLogUID >> 52) & 0xff;
        mcsfLogUid.iErrorLevel = (iLogUID >> 48) & 0x0f;
        mcsfLogUid.iPhysicalComponent = (iLogUID >> 40) & 0xff;
        mcsfLogUid.iLogicalComponent = (iLogUID >> 32) & 0xff;
        mcsfLogUid.iSpecialCode = iLogUID & 0xffffffff;
        return mcsfLogUid;
    }

    void GSetLocalLogPath(const std::string &sPath)
    {
        std::string localPath = boost::algorithm::trim_copy(sPath);
        if (!localPath.empty())
        {
            g_sLocalLogPath = localPath;
        }
    }

    void GSetLocalLogPath(const char *pPath)
    {
        if (NULL != pPath)
        {
            GSetLocalLogPath(std::string(pPath));
        }
    }

    void GSetMachineName(const std::string& sMachName)
    {
        std::string machineID = boost::algorithm::trim_copy(sMachName);
        if (!machineID.empty())
        {
            g_sMachineName = machineID;
        }
    }

    void GSetMachineName(const char* pMachName)
    {
        if (NULL != pMachName)
        {
            GSetMachineName(std::string(pMachName));
        }
    }

}