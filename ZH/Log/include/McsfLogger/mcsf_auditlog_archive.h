#ifndef __mcsf_auditlog_archive_h__
#define __mcsf_auditlog_archive_h__

#include "boost/thread.hpp"
#include "boost/filesystem.hpp"	   
#include "mcsf_logviewer_interface_common.h"

namespace Mcsf { 

	namespace LogViewerInterface {

	enum ArchiveType
	{
		NULLTYPE   = 0x00,
		NOARCHIVED = 0x01,
		SERVER     = 0x02,
		USB	       = 0x04,
		NET        = 0x08,
		ALL        = 0xFF
	};

	struct AuditLogItem
	{ 
		uint64_t date;
		char name[16];
		ArchiveType archiveType;
	};

	struct AuditLogDailyRecord
	{
		AuditLogDailyRecord():date(0),archivedUSBNum(0),
			archivedServerNum(0),notArchivedNum(0){};
		uint64_t date;
		unsigned short  archivedUSBNum;
		unsigned short  archivedServerNum;
		unsigned short  notArchivedNum;
	};
	
	//callback defined by front end user
	typedef void (__stdcall * ArchiveCallback)(uint64_t date, ArchiveType lastType, ArchiveType currentType, bool isDelete);

	class MCSF_LOGVIEWER_INTERFACE AuditLogArchive
	{
	public:

		AuditLogArchive();

		bool queryLogByDateRange(uint64_t beginDate, uint64_t endDate, AuditLogDailyRecord records[], uint64_t& len);

		//void archiveLogsByDateRange(uint64_t beginDate, uint64_t endDate, ArchiveType typeSeleted, ArchiveType archiveType, bool bDelete, ArchiveCallback callBack);

		bool archiveLogsByDateRange(uint64_t beginDate, uint64_t endDate, ArchiveType typeSeleted, ArchiveType archiveType, std::string path, bool bDelete, char errorMsg[], ArchiveCallback callBack = NULL);

		bool deleteLogsByDateRange(uint64_t beginDate, uint64_t endDate, ArchiveType typeSeleted,  char errorMsg[]);

	protected:

		bool updateAuditLogRecords();

		void getAllFileInDir(boost::filesystem::path path);

		ArchiveType getFileArchivedType(boost::filesystem::path path);

		void renameFile(boost::filesystem::path path, ArchiveType archiveType);

		std::string getBaseNameForFile(boost::filesystem::path path);

		std::string getFileName(boost::filesystem::path path);

		void removeOneLogRecord(uint64_t date, ArchiveType archiveType);
		
		void updateOneLogRecord(uint64_t date, ArchiveType lastType, ArchiveType currentType);

	private:
		std::map<uint64_t, AuditLogDailyRecord>  m_mapLogRecord;
		std::map<std::time_t, boost::filesystem::path>	m_mapAllFileInOneDir;
		uint64_t m_beginDate;
		uint64_t m_endDate;
		std::string m_logBasePath;
		boost::mutex m_updateRecordsMutex;
		boost::mutex m_operationMutex;
	};

	} // LogViewerInterface
}  //Mcsf

#endif