#ifndef mcsf_log_viewer_do_query_h__
#define mcsf_log_viewer_do_query_h__

#include "mcsf_log_viewer_types.h"
#include "mcsf_log_viewer_query_condition.h"

#include <Windows.h>
#include <list>
#include <vector>
#include <string>
#include <xstring>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>


namespace Mcsf { namespace LogViewerInterface {

#define MSEC_OF_DAY 86400000

class DoQueryThread
{
public:
	explicit DoQueryThread(const QueryCond& qryCond, bool bLogViewer, bool bReverse);

	~DoQueryThread();

	void GetResultSet(int nStartNum, int nNumOfLogs, std::list<bin_log_t>& logList);
	
	void RunSortByCond(const SortCond & sortCond);

	void StopSortThrd();

	inline bool IsSortRunning() const;

	inline uint32_t GetResultSetSize();

	inline bool IsThreadRunning() const;

	char GetBuforSvcCode(uint8_t iBuOfLogUID);

	std::string GetSvcCode(uint64_t iLogUID);

private:
	void RunSort(const SortCond & sortCond);

	inline void Start();

	inline void SetInterruptFlag(bool bInterrupt);

	void RunQuery();

	void ReadOneIndexFile(const std::string &strIndexFilePath, 
		const int nCurDate, const int nCurFileName);

private:
    boost::mutex m_fsMtx;
    bool m_bLogViewer;
    std::string m_logBufferFile;
    volatile uint32_t m_iLogCount;
    std::fstream m_fs;
    bin_log_t *m_pLogToBeView;
	QueryCond m_QryCond;
	boost::thread m_SearchThrd;
	std::list<bin_log_t> m_QualifiedLogList;
	boost::mutex m_ListMutex;
	bool m_bIsThrdRunning;
	bool m_bInterruptThrd;
	boost::thread m_SortThrd;
	bool m_bIsSortRunning;
	bool m_bReverse;
	static std::string m_deleteLogString;

};

typedef boost::shared_ptr<DoQueryThread> DoQueryThreadPtr;

void DoQueryThread::Start()
{
	m_bIsThrdRunning = true;
	m_SearchThrd = boost::thread(boost::bind(&DoQueryThread::RunQuery, this));
}

void DoQueryThread::SetInterruptFlag(bool bInterrupt)
{
	m_bInterruptThrd = bInterrupt;
}

uint32_t DoQueryThread::GetResultSetSize()
{
	/// std::list<T>::push_back won't invalidate iterators
	/// so we don't lock this operation
    if(m_bLogViewer)
    {
        return m_iLogCount;
    }
	boost::mutex::scoped_lock lock(m_ListMutex);
	return static_cast<uint32_t>(m_QualifiedLogList.size());
}

bool DoQueryThread::IsThreadRunning() const
{
	return m_bIsThrdRunning;
}

bool DoQueryThread::IsSortRunning() const
{
	return m_bIsSortRunning;
}

}}

#endif // mcsf_log_viewer_do_query_h__
