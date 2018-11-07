#include "stdafx.h"
#include "mcsf_log_file.h"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include <time.h>
#include <string>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/date_time.hpp>
#include <boost/algorithm/string.hpp>
#include <direct.h>

#include "mcsf_systemenvironment_factory.h"

#pragma warning(disable:4996)

static const boost::uint64_t time_shift = 116444736000000000ULL; // (27111902 << 32) + 3577643008

NS_BEGIN
    static const std::string Log_Path_Prefix= "./log/";
    static const std::string SvcLog_Path_Prefix= Log_Path_Prefix + "servicelog/";
    static const std::string AuditLog_Path_Prefix= Log_Path_Prefix + "auditlog/";
    static const std::string Log_Suffix = ".uihlog";
    static const std::string SvcLog_Suffix = ".svc.uihlog";
    static const std::string AuditLog_Suffix = ".audit.uihlog";

    //static const std::string indexfile_path="../log/index/";
    static const std::string svcindexfile_path= SvcLog_Path_Prefix + "index/";
    static const std::string auditIndexfile_path= AuditLog_Path_Prefix + "index/";


	LogFile* LogFile::_instance = NULL;
	static boost::mutex g_mx;
	LogFile::LogFile()
	: _file_size(0)
	, _svc_file_size(0)
	,_audit_file_size(0)
	, _max_file_size(20000000)
	, _cur_file_index(-1)
	,_cur_svcfile_index(-1)
	,_cur_auditfile_index(-1)
	,_strLogFileHeader("")
	,log_buffer_len(0)
	,svclog_buffer_len(0)
	,auditlog_buffer_len(0)
	,colon_num(0)
{
	log_buffer.reset(new char[MAX_BUFFER_SIZE]);
	svclog_buffer.reset(new char[MAX_BUFFER_SIZE]);
	auditlog_buffer.reset(new char[MAX_BUFFER_SIZE]);
	Init();
}

LogFile::~LogFile()
{
    svcMachine_index_set.clear();
    svcsource_index_set.clear();

    fclose(fs_svcMachineNm_index);
    fclose(fs_svcsource_index);
}

LogFile* LogFile::instance()
{
	boost::mutex::scoped_lock lc(g_mx);
	static LogFile factory;
	return &factory;
}

void LogFile::Write(CharArray log, std::size_t length, uint64_t &delta_time )
{
    boost::mutex::scoped_lock lock(_mutex);
	try
	{
		char *start = log.get();
		uint64_t time_tmp=0;
		char* line = start;
		size_t log_size = 1;
		size_t current_pos = 0;
		size_t tail_length = 0;

		memset(logItemtail, 0, 32);
		for (; current_pos<length-1; ++current_pos, ++log_size)
		{
			if (*(start + current_pos) == LOG_TERMINATOR&& *(start + current_pos + 1) == '\n')
			{
				////提取时间并添加时间戳
				sscanf(line, "%*[^\x2]\x2%llu\x2", &time_tmp);
				sprintf(logItemtail, "\002%llu\001\n", time_tmp + delta_time);
				tail_length=strlen(logItemtail);
				
				if(memcmp(line, TIME_SYNC_HEAD, 13) == 0 )
				{
					delta_time = GetCurrentTimeStamp64() - time_tmp;
				}
				else if(memcmp(line, AUDIT_LOG_HEAD, 14) != 0 )
				{
					if ((log_buffer_len + tail_length + log_size - 1) >= MAX_BUFFER_SIZE)
					{
						WriteLogFile();
					}

					memcpy(log_buffer.get() + log_buffer_len, line, log_size - 1);
					log_buffer_len += log_size - 1;
					memcpy(log_buffer.get() + log_buffer_len, logItemtail, tail_length);
					log_buffer_len += tail_length;

					if(memcmp(line, SVC_LOG_HEAD, 7) == 0)
					{
						GetQueryIndex(line);

						if ((svclog_buffer_len + tail_length + log_size - 1) >= MAX_BUFFER_SIZE)
						{
							WriteSvcLogFile();
						}

						memcpy(svclog_buffer.get() + svclog_buffer_len, line, log_size-1);
						svclog_buffer_len += log_size - 1;
						memcpy(svclog_buffer.get() + svclog_buffer_len, logItemtail, tail_length);
						svclog_buffer_len += tail_length;
					}
				}
				else if(memcmp(line, AUDIT_LOG_HEAD, 14) == 0)
				{
					if ((auditlog_buffer_len + log_size - 1) >= MAX_BUFFER_SIZE)
					{
						WriteLogFile(_audit_file_stream, AuditLog_Path_Prefix,  AuditLog_Suffix, _cur_auditfile_index, _audit_file_size, auditlog_buffer, auditlog_buffer_len);
					}

					memcpy(auditlog_buffer.get() + auditlog_buffer_len, line, log_size-1);
					auditlog_buffer_len += log_size - 1;
					memcpy(auditlog_buffer.get() + auditlog_buffer_len, logItemtail, tail_length);
					auditlog_buffer_len += tail_length;
				}

				line=start + (current_pos + 2);
				++current_pos;
				log_size = 0;
			}
		}

		if (svclog_buffer_len)
		{
			WriteSvcLogFile();
			

			UpdateQueryIndexFile(fs_svcMachineNm_index, svcMachine_index_set, _svcMachine_index_set_);
			UpdateQueryIndexFile(fs_svcsource_index, svcsource_index_set, _svcsource_index_set_);
		}

		if (log_buffer_len)
		{
			WriteLogFile();
		}	

		if (auditlog_buffer_len)
		{
			WriteLogFile(_audit_file_stream, AuditLog_Path_Prefix,  AuditLog_Suffix, _cur_auditfile_index, _audit_file_size, auditlog_buffer, auditlog_buffer_len);
		}	
	}
	catch(std::exception &e)
	{
		printf("LogFile::Write=>%s\n", e.what());
	}
	catch(...)
	{
		printf("LogFile::Write=>unknown exception.");
	}

}

void LogFile::Init()
{
    if(!boost::filesystem::exists(svcindexfile_path))
        boost::filesystem::create_directories(svcindexfile_path);

	if(!boost::filesystem::exists(auditIndexfile_path))
		boost::filesystem::create_directories(auditIndexfile_path);

    fs_svcMachineNm_index = ReadFileToSet(svcindexfile_path + "machine", svcMachine_index_set);
    fs_svcsource_index = ReadFileToSet(svcindexfile_path + "source", svcsource_index_set);
}

FILE* LogFile::ReadFileToSet(const std::string&  filename, std::set<std::string>& index_set)
{
		FILE *fp = NULL;
		char log_item[ITEM_LENGTH];
		if (NULL != (fp = fopen(filename.c_str(), "ab+")))
		{
			memset(log_item, 0, ITEM_LENGTH);

			while( (fgets(log_item, ITEM_LENGTH, fp) != NULL))
			{
				log_item[strlen(log_item)-1] = '\0';
				index_set.insert(log_item);
			}
		}
		return fp;
}

void LogFile::UpdateQueryIndexFile(FILE *fsquery_index,
    std::set<std::string>& original_index_set,
    std::set<std::string>& new_index_set)
{	
    if (NULL == fsquery_index)
        return;

    for (set<std::string>::iterator _set_iter_ = new_index_set.begin();
        _set_iter_ != new_index_set.end(); _set_iter_++)
    {				
        std::string item_tmp=*_set_iter_;
        if (original_index_set.end() == original_index_set.find(item_tmp))
        {
            original_index_set.insert(item_tmp);
            item_tmp.append("\n");
            fwrite(item_tmp.c_str(), 1, item_tmp.size(), fsquery_index);
        }
    }

    new_index_set.clear();
    fflush(fsquery_index);
}

void LogFile::GetQueryIndex(const char* log_line)
{
	memset(machine_tmp, 0, ITEM_LENGTH);
	memset(source_tmp, 0, ITEM_LENGTH);
	memset(item_block, 0, ITEM_LENGTH);
	colon_num = 0;
	sscanf(log_line, "%*[^\x2]\x2%*[^\2]\x2%[^(]%[^\x2]", source_tmp, item_block);

	for (int i = 0; (item_block[i] != ')')&&(i<strlen(item_block)); i++)
	{
		if(':'==item_block[i])
			colon_num++;
	}

	if (2==colon_num)
	{
		sscanf(item_block,"(%[^:]",machine_tmp);
		_svcMachine_index_set_.insert(machine_tmp);
	}

    _svcsource_index_set_.insert(source_tmp);

}

void LogFile::WriteSvcLogFile()
{
	_tzset();
    static boost::gregorian::date lastDate = boost::gregorian::day_clock::local_day();

    if(!_svc_file_stream.is_open())
        reset_file(SvcLog_Path_Prefix, SvcLog_Suffix
        ,_cur_svcfile_index, _svc_file_stream, _svc_file_size, lastDate);

    _svc_file_stream.write(svclog_buffer.get(), svclog_buffer_len);
    _svc_file_stream.flush();

    _svc_file_size += svclog_buffer_len;

    if(_svc_file_size > _max_file_size || (lastDate != boost::gregorian::day_clock::local_day()))
        reset_file(SvcLog_Path_Prefix, SvcLog_Suffix
        ,_cur_svcfile_index, _svc_file_stream, _svc_file_size, lastDate);

    //memset(svclog_buffer.get(), 0, MAX_BUFFER_SIZE);
    svclog_buffer_len = 0;
}

void LogFile::WriteLogFile(std::ofstream& file_stream, const std::string& Log_Path_Prefix, const std::string& Log_Suffix,
	                       int&  cur_file_index, std::size_t& file_size, CharArray& log_buffer, size_t& log_buffer_len)
{
	_tzset();
	static boost::gregorian::date lastDate = boost::gregorian::day_clock::local_day();

	if(!file_stream.is_open())
		reset_file(Log_Path_Prefix, Log_Suffix
		, cur_file_index, file_stream, file_size, lastDate);

	file_stream.write(log_buffer.get(), log_buffer_len);
	file_stream.flush();

	file_size += log_buffer_len;

	if(file_size > _max_file_size || (lastDate != boost::gregorian::day_clock::local_day()))
		reset_file(Log_Path_Prefix, Log_Suffix
		, cur_file_index, file_stream, file_size, lastDate);

	log_buffer_len=0;
}

void LogFile::WriteLogFile()
{
	_tzset();
    static boost::gregorian::date lastDate = boost::gregorian::day_clock::local_day();

    if(!_file_stream.is_open())
        reset_file(Log_Path_Prefix, Log_Suffix
        , _cur_file_index, _file_stream, _file_size, lastDate);

    _file_stream.write(log_buffer.get(), log_buffer_len);
    _file_stream.flush();

    _file_size += log_buffer_len;

    if(_file_size > _max_file_size || (lastDate != boost::gregorian::day_clock::local_day()))
        reset_file(Log_Path_Prefix, Log_Suffix
        , _cur_file_index, _file_stream, _file_size, lastDate);

    //memset(log_buffer.get(), 0, MAX_BUFFER_SIZE);
    log_buffer_len=0;
}

void LogFile::reset_file(const std::string & dir, const std::string &suffix
    ,int &file_index,std::ofstream &file_stream,std::size_t &file_size, boost::gregorian::date &lastDate)
{
    try
    {
        file_size = 0;
        boost::gregorian::date dtToday = boost::gregorian::day_clock::local_day();

        if (lastDate !=  dtToday)
            file_index = -1;

        std::string sdate = boost::gregorian::to_iso_extended_string( dtToday );
        boost::filesystem::path filePath(dir + sdate + "/");

        if(!boost::filesystem::exists(filePath))
            boost::filesystem::create_directories(filePath);

        if(file_index == -1)
        {
            boost::filesystem::directory_iterator dirIter(filePath), dirEnd;
            boost::regex e(".+\\.*log$");
            for(; dirIter != dirEnd; ++dirIter)
            {
                std::string sPath = dirIter->path().leaf().string();
                if(boost::filesystem::is_regular_file(*dirIter)
                    && boost::regex_match(sPath, e))
                {
                    int iTmp = 0;
                    sscanf(sPath.c_str(), "%d", &iTmp);
                    if(iTmp > file_index)
                        file_index = iTmp;
                }
            }
        }

        ++file_index;

        filePath /= (boost::lexical_cast<std::string>(file_index) + suffix);

        file_stream.close();
        file_stream.open(filePath.string().c_str(), std::ofstream::app |std::ofstream::binary);
        //write log file header info
        file_size = _strLogFileHeader.size();
        file_stream.write(_strLogFileHeader.c_str(), file_size);
        file_stream.flush();

        lastDate = dtToday;

    }
    catch (...)
    {
        std::cout << "exception in func:" << __FUNCTION__ << "!" << std::endl;
    }
}

void LogFile::setLogFileMaxSize(std::size_t max_size)
{
    _max_file_size = max_size;
}


void LogFile::SetLogFileHeader(const std::string &strLogFileHeader)
{
    _strLogFileHeader = strLogFileHeader;
}

boost::uint64_t LogFile::GetCurrentTimeStamp64()
{
#ifdef MCSF_PLATFORM_POSIX
	timeval tv;
	gettimeofday(&tv, 0); //gettimeofday does not support TZ adjust on Linux.
	return (static_cast<boost::uint64_t>(tv.tv_sec)) * 1000 + tv.tv_usec / 1000;
#elif defined(MCSF_PLATFORM_WIN32)
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);	
	union {
		FILETIME as_file_time;
		boost::uint64_t as_integer; // 100-nanos since 1601-Jan-01
	} time_caster;
	time_caster.as_file_time = ft;
	time_caster.as_integer -= time_shift; // filetime is now 100-nanos since 1970-Jan-01
	return time_caster.as_integer / 10000UL; //in millisecond and cannot be before 1970-Jan-01
#else
	time_t seconds;
	return time(NULL) * 1000UL;
#endif
}
NS_END

