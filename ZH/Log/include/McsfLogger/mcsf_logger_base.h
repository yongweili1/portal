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
#include "mcsf_logger_common.h"
#include <set>
#include <string>
#include <deque>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
#if defined(MCSF_PLATFORM_WIN32)
#pragma warning(disable:4127)
#endif

#ifndef USER_DATATYPE_LOGGER_NAMESPACE
#define USER_DATATYPE_LOGGER_NAMESPACE
namespace boost
{
    namespace posix_time
    {
        class ptime;
    }
}

namespace LogTypes {
#ifdef MCSF_PLATFORM_POSIX
    __extension__ typedef long long int64_t;
    __extension__ typedef unsigned long long uint64_t;
#else
    typedef __int64   int64_t;
    typedef unsigned __int64   uint64_t;
#endif

    typedef unsigned char uint8_t;

    typedef unsigned int uint32_t;
}
#endif


#ifdef USER_NAMESPACE_DEFINED
#undef USER_NAMESPACE_DEFINED
#endif
#define USER_NAMESPACE_DEFINED Mcsf
#ifdef NAMESPACE_FLAG
#undef NAMESPACE_FLAG
#endif
#define NAMESPACE_FLAG 1
#ifndef MCSF_LOGGER_BASE_NON_RT
#define MCSF_LOGGER_BASE_NON_RT 0
#else
#undef MCSF_LOGGER_BASE_NON_RT
#define MCSF_LOGGER_BASE_NON_RT 1
#endif

#if defined(NAMESPACE_FLAG)
#if (NAMESPACE_FLAG == 0 && defined(MCSF_LOGGER_BASE_RT) && MCSF_LOGGER_BASE_RT == 0) \
  || (NAMESPACE_FLAG == 1 && defined(MCSF_LOGGER_BASE_NON_RT) && MCSF_LOGGER_BASE_NON_RT == 0)
namespace USER_NAMESPACE_DEFINED {

    enum LogType {
        NORMAL_LOG = 0,
        AUDIT_LOG
    };

    ///brief:  Get Logger Instance and Init Global Parameter
    ///param:
    ///       [IN] const std::string& sConfigXMLFile: xml file path in string
    ///       [IN] const char* sConfigXMLFile: xml file path in char*
    void    MCSF_LOGGER_API       GLogLoadConfig(const std::string& sConfigXMLFile);
    extern "C" MCSF_LOGGER_API  void GLogLoadConfig(const char* sConfigXMLFile);

    extern "C" MCSF_LOGGER_API void GLogWriteToBuffer(const char* log);


    ///brief: Check if need to launch the log local transfer.
    ///param: VOID
    ///retval: true for need, while false for no need.
    bool    MCSF_LOGGER_API       GLogIsLocalTransferTobeLaunched();

    ///brief: stop sending log
    /// param: VOID
    /// retval: true for success, otherwise for failure.
    extern "C"  MCSF_LOGGER_API  bool   GLogUnloadConfig();
    bool    MCSF_LOGGER_API       GLogFlush();
    ///brief: indicate name of folder in which log file is contained
    ///param: 
    ///        [IN] const std::string& sSource: log folder name in string
    ///        [IN] const char* sSource: log folder name in const char*
    ///retval:  void
    void  MCSF_LOGGER_API  GLogSetSource(const std::string& sSource);
    extern "C" MCSF_LOGGER_API void GLogSetSource(const char* sSource);

    ///brief: get sub string(in format:<source_name>(<process_ID>:<thread_ID>))
    ///        to be inserted in log entry
    ///param:
    ///        [IN] const std::string& sSource: indicate the source name
    ///retval: the combined substring 
    std::string MCSF_LOGGER_API   GLogGetSource(const std::string& sSource, LogType type = NORMAL_LOG);

    ///brief: set current trace level
    ///param: 
    ///           [IN] int iLevel: the trace level to be set
    ///retval: if iLevel is valid ,return true ,otherwise false;
    bool    MCSF_LOGGER_API       GSetTraceLevel(int iLevel);

    //brief: reset the trace level to default value
    ///param: 
    ///          [IN] void
    ///retval:    void
    void    MCSF_LOGGER_API       GResetTraceLevel();

    ///brief: get current trace level
    ///param:
    ///        [IN] void
    ///retval: current trace level value  
    int     MCSF_LOGGER_API       GGetTraceLevel();

    bool    MCSF_LOGGER_API       GIsTraceInfoOn();
    bool    MCSF_LOGGER_API       GIsTraceWarningOn();
    bool    MCSF_LOGGER_API       GIsTraceErrorOn();
    bool    MCSF_LOGGER_API       GIsPerformanceRecordOn();

    void	MCSF_LOGGER_API		  GSetLogTraceInfo(bool on);
    void	MCSF_LOGGER_API		  GSetLogTraceWarning(bool on);
    void	MCSF_LOGGER_API		  GSetLogTraceError(bool on);


    ///brief:bOutputFlag  true-Write on Console; false-Write on Socket(Default)
    ///param:
    ///           [IN] bool bOutputFlag: the output flag
    ///retval: void
    void    MCSF_LOGGER_API       GSetOutputFlag(bool bOutputFlag);

    class MCSF_LOGGER_API CLog
    {
    public:

        static bool IsLogSwitchOn(const::string& sLogLevel);
        CLog(const std::string& sSource, const LogTypes::uint64_t ullLogUid,
            const char* pfile, const char* pfun, int lineNum, const char* level
            , bool enable = true, LogType type = NORMAL_LOG);

        CLog(const char* sSource, const LogTypes::uint64_t ullLogUid,
            const char* pfile, const char* pfun, int lineNum, const char* level
            , bool enable = true, LogType type = NORMAL_LOG);

        CLog(const std::string& sSource, const LogTypes::uint64_t ullLogUid,
            const char* pfile, const char* pfun, int lineNum, const std::string& sLogLevel
            , const std::string& stime);

        CLog(const char* sSource, const LogTypes::uint64_t ullLogUid,
            const char* pfile, const char* pfun, int lineNum, const char* sLogLevel
            , const char* stime);

        virtual ~CLog();
    public:
        template<class T>
        CLog& operator<< (T val)
        {
            _stream << val;
            return *this;
        }
    protected:
        ///brief: construct a log entry ,and put it into log data queue
        ///param:
        ///            void
        ///retval:    void
        void send();

    public:
        inline stringstream& stream()
        {
            return _stream;
        }

    protected:
        bool			_enable;
        char*			_log;
        std::size_t		_length;
        std::size_t		_log_max_length;
        std::size_t		_log_content_max_length;
        std::stringstream _stream;
        LogType         _log_type;

    };

    //derived from class CLog
    class MCSF_LOGGER_API CLogTraceError : public CLog
    {
    public:
        CLogTraceError(const std::string& sSource, const LogTypes::uint64_t ulLogUid,
            const char* pfile, const char* pfun, int lineNum, const char* level);
    };

    //derived from class CLog
    class MCSF_LOGGER_API CLogTraceInfo : public CLog
    {
    public:
        CLogTraceInfo(const std::string& sSource, const LogTypes::uint64_t ulLogUid,
            const char* pfile, const char* pfun, int lineNum, const char* level);
    };

    //derived from class CLog
    class MCSF_LOGGER_API CLogTraceWarning : public CLog
    {
    public:
        CLogTraceWarning(const std::string& sSource, const LogTypes::uint64_t ulLogUid,
            const char* pfile, const char* pfun, int lineNum, const char* level);
    };

    //derived from class CLog
    class MCSF_LOGGER_API CLogAuditLog : public CLog
    {
    public:
        CLogAuditLog(const std::string& sSource, const LogTypes::uint64_t ulLogUid,
            const char* pfile, const char* pfun, int lineNum, const char* level);
    };

    //now unused
    class MCSF_LOGGER_API CLogDumpToFile
    {
    public:
        CLogDumpToFile();
        ~CLogDumpToFile() {}
    };

    //used to get elapsed time of constructing a log entry
    class MCSF_LOGGER_API CLogExecutingTime// : public CLog
    {
    public:
        CLogExecutingTime(const char* sSource, const char* pfile
            , const char* pfun, int lineNum);
        CLogExecutingTime(const char* sSource, const char* pfile
            , const char* pfun, int lineNum, const char* name);
        ~CLogExecutingTime();
    private:
        LogTypes::int64_t				m_perf_couter_begin;
        const char*		_filename;
        int				_linenum;
        const char*		_funname;
        const char*		_source;
        const char*		_name;
        bool            _trace_on;
    };


    typedef struct MCSF_LOGGER_API MCSFLOGUID {
        LogTypes::uint8_t iBU;
        LogTypes::uint8_t iModality;
        LogTypes::uint8_t iErrorLevel;
        LogTypes::uint8_t iPhysicalComponent;
        LogTypes::uint8_t iLogicalComponent;
        LogTypes::uint32_t iSpecialCode;
    } sMcsfLogUID;

    //////////////////////////////////////////////////////////////////////////
    /// \brief: translate log uid to service code
    /// \[in] uint64_t iLogUID 
    ///          note: record some info(bu, modality,error code and so on.)
    /// \ret: string 
    ///          note: service code translated from iLogUID
    //////////////////////////////////////////////////////////////////////////
    std::string MCSF_LOGGER_API GGetServiceCode(LogTypes::uint64_t iLogUID);

    //////////////////////////////////////////////////////////////////////////
    /// \brief: generate LOGUID from all its parts
    /// \[in] 
    /// \ret: uint64_t
    ///           note: the combined LOGUID from all its parts
    //////////////////////////////////////////////////////////////////////////
    LogTypes::uint64_t MCSF_LOGGER_API GMakeLogUID(LogTypes::uint8_t iBU, LogTypes::uint8_t iModality, LogTypes::uint8_t iErrorLevel,
        LogTypes::uint8_t iPhysicalComponent, LogTypes::uint8_t iLogicalComponent, LogTypes::uint32_t iSpecialCode);

    //////////////////////////////////////////////////////////////////////////
    /// \brief: parse LOGUID to get all parts
    /// \[in] uint64_t
    ///           note: loguid to be parsed
    /// \ret: sMcsfLogUID
    ///           note: all parts of a complete loguid field
    //////////////////////////////////////////////////////////////////////////
    sMcsfLogUID MCSF_LOGGER_API GParseLogUID(LogTypes::uint64_t iLogUID);

    void MCSF_LOGGER_API GSetLocalLogPath(const std::string &sPath);
    extern "C" MCSF_LOGGER_API void GSetLocalLogPath(const char *pPath);

    void MCSF_LOGGER_API GSetMachineName(const std::string& sMachName);
    extern "C" MCSF_LOGGER_API void GSetMachineName(const char* pMachName);

}//end namespace
#endif
#endif // mcsf_logger_global_h__
