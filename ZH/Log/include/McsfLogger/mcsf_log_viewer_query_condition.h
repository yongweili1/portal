#ifndef mcsf_log_viewer_query_condition_h__
#define mcsf_log_viewer_query_condition_h__

#include "mcsf_logviewer_interface_common.h"

#include <set>
#include <vector>
#include <string>
#include <cstdint>
#include <boost/shared_ptr.hpp>

namespace Mcsf { namespace LogViewerInterface {

class MCSF_LOGVIEWER_INTERFACE QueryBase
{
public:
	virtual bool Verify(void* val) const = 0;
};

template <typename T>
class MCSF_LOGVIEWER_INTERFACE QueryEqual : public QueryBase
{
public:
	QueryEqual()
	{
		m_CondSet.reserve(32);
	}
	~QueryEqual()
	{
		Clear();
	}

	bool Verify(void* val) const
	{
		//return (m_CondSet.count(*(T*)val) > 0);
		return std::binary_search(m_CondSet.begin(), m_CondSet.end(), *(T*)val);
	}

	void Insert(const T& val)
	{
		m_CondSet.push_back(val);
	}

	bool IsNull() const
	{
		return m_CondSet.empty();
	}

	void Remove(const T& val)
	{
		std::vector<T>::iterator iter;
		iter = std::find(m_CondSet.begin(), m_CondSet.end(), val);
		if (iter != m_CondSet.end())
		{
			m_CondSet.erase(iter);
		}
	}

	void Clear()
	{
		m_CondSet.clear();
	}
	
	void Traverse(std::vector<T> &condSet)
	{
		condSet = m_CondSet;
	}

	void Sort()
	{
		std::sort(m_CondSet.begin(), m_CondSet.end());
		std::vector<T>::iterator iter = std::unique(m_CondSet.begin(), m_CondSet.end());
		m_CondSet.erase(iter, m_CondSet.end());
	}

	bool operator==(const QueryEqual<T>& rhs) const
	{
		if (this->m_CondSet.size() != rhs.m_CondSet.size()) return false;

		std::vector<T>::const_iterator lhsIter = this->m_CondSet.begin();
		std::vector<T>::const_iterator rhsIter = rhs.m_CondSet.begin();
		while (lhsIter != this->m_CondSet.end())
		{
			if (*lhsIter++ == *rhsIter++) continue;
			return false;
		}

		return true;
	}
	bool operator!=(const QueryEqual<T>& rhs) const
	{
		return !(*this == rhs);
	}

	//// Compare function for associative container
	//// By the way string is compared
	//bool operator<(const QueryEqual<T>& rhs) const;


private: 
	//std::set<T> m_CondSet;
	std::vector<T> m_CondSet;
};

template <typename T>
class MCSF_LOGVIEWER_INTERFACE QueryScope : public QueryBase
{
public:
	bool Verify(void* val) const
	{
		return (*(T*)(val) >= m_LeftVal) && (*(T*)(val) <= m_RightVal);
	}

	QueryScope(T val1 = T(), T val2 = T())
		: m_LeftVal(val1), m_RightVal(val2)
	{

	}

	bool IsNull() const
	{
		return m_LeftVal == T() && m_RightVal == T();
	}

	T GetLeftValue() const
	{
		return m_LeftVal;
	}

	T GetRightValue() const
	{
		return m_RightVal;
	}

	bool operator==(const QueryScope<T>& rhs) const
	{
		return m_LeftVal == rhs.m_LeftVal && m_RightVal == rhs.m_RightVal;
	}

	bool operator!=(const QueryScope<T>& rhs) const
	{
		return !(*this == rhs);
	}

	//// Compare function for associative container
	//bool operator<(const QueryScope<T>& rhs) const
	//{
	//	return m_LeftVal < rhs.m_LeftVal;.
	//}
private: 
	T m_LeftVal;
	T m_RightVal;
};

template <typename T>
class MCSF_LOGVIEWER_INTERFACE QueryContain : public QueryBase
{
public:
	QueryContain() : m_bWildCharProcessed(false)
	{
	}

	bool Verify(void* val) const;

	void Insert(const T& val)
	{
		m_CondSet.insert(val);
	}

	bool IsNull() const
	{
		return m_CondSet.empty();
	}

	void Remove(const T& val)
	{
		m_CondSet.erase(val);
	}

	void Clear()
	{
		m_CondSet.clear();
	}

	void Traverse(std::set<T> &condSet)
	{
		condSet = m_CondSet;
	}

	bool operator==(const QueryContain<T>& rhs) const;
	bool operator!=(const QueryContain<T>& rhs) const
	{
		return !(*this == rhs);
	}

    void ProcessWildChar() const;

private: 
	std::set<T> m_CondSet;
    mutable bool m_bWildCharProcessed;
};

typedef struct MCSF_LOGVIEWER_INTERFACE _query_cond_t
{
	QueryEqual<uint8_t> m_LevelCond;
	QueryScope<uint64_t> m_TimeCond;
	QueryEqual<uint16_t> m_McnmCond;
	QueryEqual<uint32_t> m_PIDCond;
	QueryEqual<uint32_t> m_TIDCond;
	QueryEqual<uint16_t> m_SrcCond;
	QueryEqual<uint32_t> m_SrcFileCond;
	QueryEqual<uint32_t> m_FuncCond;
	QueryEqual<uint16_t> m_LineCond;
	QueryEqual<uint64_t> m_UIDCond;
	QueryContain<std::string> m_SvcCodeCond;
	QueryContain<std::string> m_DescCond;

	_query_cond_t()
	{
		/// instantiate the template member functions
		m_LevelCond.Insert(0);
		m_LevelCond.Remove(0);
		m_LevelCond.Clear();
		std::vector<uint8_t> levelCondSet;
		m_LevelCond.Traverse(levelCondSet);
		m_LevelCond.Sort();

		m_PIDCond.Insert(0);
		m_PIDCond.Remove(0);
		m_PIDCond.Clear();
		std::vector<uint32_t> PIDCondSet;
		m_PIDCond.Traverse(PIDCondSet);
		m_PIDCond.Sort();

		m_SrcFileCond.Insert(0);
		m_SrcFileCond.Remove(0);
		m_SrcFileCond.Clear();
		std::vector<uint32_t> srcFileCondSet;
		m_SrcFileCond.Traverse(srcFileCondSet);
		m_SrcFileCond.Sort();

		m_McnmCond.Insert(0);
		m_McnmCond.Remove(0);
		m_McnmCond.Clear();
		std::vector<uint16_t> McnmSet;
		m_McnmCond.Traverse(McnmSet);
		m_McnmCond.Sort();

		m_UIDCond.Insert(0);
		m_UIDCond.Remove(0);
		m_UIDCond.Clear();
		std::vector<uint64_t> UIDCondSet;
		m_UIDCond.Traverse(UIDCondSet);
		m_UIDCond.Sort();

		m_SvcCodeCond.Insert("SvcSample");
		m_SvcCodeCond.Remove("SvcSample");
		m_SvcCodeCond.Clear();
		std::set<std::string> SvcCodeCondSet;
		m_SvcCodeCond.Traverse(SvcCodeCondSet);

		m_DescCond.Insert("Sample");
		m_DescCond.Remove("Sample");
		m_DescCond.Clear();
		std::set<std::string> descCondSet;
		m_DescCond.Traverse(descCondSet);
	}

	bool operator==(const _query_cond_t& rhs) const
	{
		return m_LevelCond == rhs.m_LevelCond
			&& m_TimeCond == rhs.m_TimeCond
			&& m_PIDCond == rhs.m_PIDCond
			&& m_TIDCond == rhs.m_TIDCond
			&& m_SrcCond == rhs.m_SrcCond
			&& m_SrcFileCond == rhs.m_SrcFileCond
			&& m_FuncCond == rhs.m_FuncCond
			&& m_McnmCond == rhs.m_McnmCond
			&& m_LineCond == rhs.m_LineCond
			&& m_UIDCond == rhs.m_UIDCond
			&& m_DescCond == rhs.m_DescCond;
	}

	bool operator!=(const _query_cond_t& rhs) const
	{
		return !(*this == rhs);
	}

	// Compare function for associative container
	//bool operator<(const _query_cond_t& rhs) const
	//{
	//	return ;
	//}
	// default operator= can satisfy needs

} QueryCond;
typedef boost::shared_ptr<QueryCond> QueryCondPtr;

typedef struct MCSF_LOGVIEWER_INTERFACE _sort_cond_t
{
	int iSortCriterion;
	bool bDesc;
	_sort_cond_t()
		: bDesc(false)
	{
	}
}SortCond;

/// for sort 
enum MCSF_LOGVIEWER_INTERFACE SortCriterion
{
	SORT_BY_LEVEL,
	SORT_BY_TIME,
	SORT_BY_SOURCE,
	SORT_BY_SRCFILE,
	SORT_BY_FUNC,
	SORT_BY_LINENO,
	SORT_BY_PID,
	SORT_BY_TID,
	SORT_BY_UID,
	SORT_BY_DESC,
	SORT_BY_MCNM
};

}}

#endif // mcsf_log_viewer_query_condition_h__
