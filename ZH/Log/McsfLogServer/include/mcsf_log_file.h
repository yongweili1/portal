#ifndef mcsf_log_file_h__
#define mcsf_log_file_h__
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/date_time.hpp>
#include <fstream>
#include <set>
#include "../McsfSharedMemory/mcsf_log_shared_memory.h"
#include "mcsf_log_common.h"

#include <mcsf_logviewer_interface.h>
#ifdef OPEN_LOGVIEWER_INTERFACE
using namespace Mcsf::LogViewerInterface;
#endif
using namespace std;
#define ITEM_LENGTH 50
#define  MAX_BUFFER_SIZE  65535
#define  LOG_MAX_LENGTH 1024
typedef boost::shared_array<char> CharArray;


NS_BEGIN

struct LogInfo
{
	std::ofstream	fileStream;
	int             curFileIndex;
	std::size_t		fileSize;
	size_t          logBufferLen;
	CharArray       logBuffer;
};

class LogFile
{
public:

    LogFile();
    virtual ~LogFile();

    virtual void Write(CharArray log, std::size_t length, uint64_t &delta_time);
    void setLogFileMaxSize(std::size_t max_size);
    void SetLogFileHeader(const std::string &strLogFileHeader);
	static LogFile* instance();

private:

    void GetQueryIndex(const char* log_line);
    void Init();
    void reset_file(const std::string & dir
        , const std::string &suffix
        ,int &file_index
        ,std::ofstream &file_stream
        ,std::size_t &file_size
        ,boost::gregorian::date &lastDate);

    FILE* ReadFileToSet(const std::string&  filename
        ,std::set<std::string>& index_set);

    void UpdateQueryIndexFile(FILE *fsquery_index
        ,std::set<std::string>& original_index_set
        ,std::set<std::string>& new_index_set);

    void WriteSvcLogFile();
    void WriteLogFile();
	void WriteAuditLogFile();
	void WriteLogFile(std::ofstream& file_stream, const std::string& Log_Path_Prefix,
		              const std::string& Log_Suffix, int&  cur_file_index, std::size_t& svc_file_size, CharArray& log_buffer, size_t& log_buffer_len);
	boost::uint64_t GetCurrentTimeStamp64();

private:

    std::string     _strLogFileHeader;
    std::size_t     _max_file_size;
    std::ofstream	_file_stream;
    std::ofstream	_svc_file_stream;
	std::ofstream	_audit_file_stream;
    int             _cur_file_index;
    int             _cur_svcfile_index;
	int             _cur_auditfile_index;
    std::size_t		_file_size;
    std::size_t		_svc_file_size;
	std::size_t		_audit_file_size;
	int              colon_num;

    boost::mutex	_mutex; 
    size_t          log_buffer_len;
    size_t          svclog_buffer_len;
	size_t          auditlog_buffer_len;

	char machine_tmp[ITEM_LENGTH];
	char source_tmp[ITEM_LENGTH];
	char item_block[ITEM_LENGTH];
	char logItemtail[32];

#ifdef OPEN_LOGVIEWER_INTERFACE
    std::fstream	_index_stream;
    log_parser      _parser;
    SharedMemory	_shared_memory;
#endif
    
    set<std::string>    svcMachine_index_set;
    set<std::string>    svcsource_index_set;

    set<std::string>    _svcMachine_index_set_;
    set<std::string>    _svcsource_index_set_;

    CharArray           log_buffer;
    CharArray           svclog_buffer;
	CharArray           auditlog_buffer;
    static LogFile*     _instance;

    FILE*               fs_svcMachineNm_index;
    FILE*               fs_svcsource_index;

};
typedef boost::shared_ptr<LogFile> LogFilePtr;

NS_END


#endif // mcsf_log_file_h__
