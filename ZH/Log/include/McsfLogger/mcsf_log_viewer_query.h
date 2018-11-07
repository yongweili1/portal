#ifndef mcsf_log_viewer_query_h__
#define mcsf_log_viewer_query_h__

#include "mcsf_logviewer_interface_common.h"
#include "mcsf_log_viewer_query_condition.h"
#include "mcsf_log_viewer_do_query.h"
#include "mcsf_log_viewer_types.h"

#include <vector>

namespace Mcsf { namespace LogViewerInterface {

class MCSF_LOGVIEWER_INTERFACE Query
{
private:
	typedef std::vector<std::pair<QueryCond, DoQueryThreadPtr> >::iterator iterator;

public:
    static void SetTypeToLogViewer();

	static void GetIndexedLog(QueryCond& queryCond, int nStartNum, 
		int nNumOfLogs, std::list<bin_log_t>& indexedLogList, bool b_Reverse = false);

	static uint32_t GetLogSum(const QueryCond& queryCond);

	static int SortResult(const QueryCond& queryCond, const SortCond& sortCond);

	inline static void StopSort(const QueryCond& queryCond);

	inline static bool IsSorting(const QueryCond& queryCond);

	inline static void StopAllThreads();

	inline static bool IsThreadRunning(const QueryCond& queryCond);

	inline static void StopThread(const QueryCond& queryCond);

private:
	static iterator FindQueryCond(const QueryCond& queryCond);

private:
	static std::vector<std::pair<QueryCond, DoQueryThreadPtr> > m_QryCondQryThrdVec;
	static boost::mutex m_VectorMutex; // for possible multiple instances
    static bool m_bLogViewer;
};

void Query::StopAllThreads()
{
	boost::mutex::scoped_lock lock(m_VectorMutex);
	/// 语义是 删除所有查询条件？还是只停止查询线程
	if (!m_QryCondQryThrdVec.empty())
	{
		m_QryCondQryThrdVec.clear();
	}
}

bool Query::IsThreadRunning(const QueryCond& queryCond)
{
	boost::mutex::scoped_lock lock(m_VectorMutex);

	iterator iter = FindQueryCond(queryCond);
	iterator end = m_QryCondQryThrdVec.end();

	if (iter == end) return false;

	return iter->second->IsThreadRunning() || iter->second->IsSortRunning();
}

void Query::StopThread(const QueryCond& queryCond)
{
#if 0
	boost::mutex::scoped_lock lock(m_VectorMutex);

	iterator iter = FindQueryCond(queryCond);
	iterator end = m_QryCondQryThrdVec.end();

	if (iter == end) return;

	m_QryCondQryThrdVec.erase(iter);
#else
    queryCond;
    StopAllThreads();
#endif
}
void Query::StopSort(const QueryCond& queryCond)
{
	boost::mutex::scoped_lock lock(m_VectorMutex);

	iterator iter = FindQueryCond(queryCond);
	iterator end = m_QryCondQryThrdVec.end();

	if (iter == end) return;

	return iter->second->StopSortThrd();
}

bool Query::IsSorting(const QueryCond& queryCond)
{
	boost::mutex::scoped_lock lock(m_VectorMutex);

	iterator iter = FindQueryCond(queryCond);
	iterator end = m_QryCondQryThrdVec.end();

	if (iter == end) return false;

	return iter->second->IsSortRunning();
}

}}

#endif // mcsf_log_viewer_query_h__