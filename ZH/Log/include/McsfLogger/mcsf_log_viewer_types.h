#ifndef mcsf_log_viewer_types_h__
#define mcsf_log_viewer_types_h__

#include "mcsf_logviewer_interface_common.h"
#include <cstdint>
#include <boost/shared_ptr.hpp>

#define MAXITEMLENGTH 512
#define ITEMLENGTH          50
#define MAXLOGLENGTH  (1024)
#define MAX_FILE_DIRECTORY  512
#define MAX_AUDITLOG_LENGTH 2048

namespace Mcsf { namespace LogViewerInterface {

	enum LogType{
		NORMAL_LOG = 0,
		AUDIT_LOG
	};

	enum EventLevel
	{
		MINOR_FAILURE = 0,
		SUCCESS
	};

    typedef struct MCSF_LOGVIEWER_INTERFACE _bin_log_t
    {
        uint64_t _time;
        uint64_t _uid;
        uint32_t _func;
        uint32_t _offset;
        uint32_t _file;
        uint32_t _date;
		uint16_t _mcnm;
        uint16_t _source;
        uint32_t _pid;
        uint32_t _tid;
        uint16_t _line;
        uint16_t _log_file;
        uint8_t _level;
    } bin_log_t;
typedef boost::shared_ptr<bin_log_t> BinLogPtr;

typedef struct MCSF_LOGVIEWER_INTERFACE _txt_log_t
{
	std::string _level;
	std::string _time;
	std::string _source;
	std::string _file;
	std::string _func;
	std::string _line;
	std::string _mcnm;
	std::string _pid;
	std::string _tid;
	std::string _uid;
	std::string _desc;
} txt_log_t;
typedef boost::shared_ptr<txt_log_t> TxtLogPtr;

typedef struct MCSF_LOGVIEWER_INTERFACE _log_for_c_sharp
{
    char _level[50];
    char _time[50];
    char  _source[50];
    char  _file[50];
    char  _func[50];
    char  _line[50];
	char  _mcnm[50];
    char  _pid[50];
    char  _tid[50];
    char  _uid[50];
    char  _desc[50];
} log_for_c_sharp;


typedef struct MCSF_LOGVIEWER_INTERFACE  _log_item_s
{
	uint32_t pid;
	uint32_t tid;
	uint64_t servertimestamp;
	char level[ITEMLENGTH];
	char source[ITEMLENGTH];
	char sourcefile[ITEMLENGTH];	
	char line[ITEMLENGTH];
	char func[ITEMLENGTH];
	char uid[ITEMLENGTH];
	char mechinename[ITEMLENGTH];
	char description[MAXITEMLENGTH];			
} log_item_s;

struct MCSF_LOGVIEWER_INTERFACE  audit_log_item
{
	char description[MAX_AUDITLOG_LENGTH];
};

}}
#endif // mcsf_log_viewer_types_h__
