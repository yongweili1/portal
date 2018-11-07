//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  xiaotao.yang (mailto:xiaotao.yang@united-imaging.com)
/// 
///  \file mcsf_log_processor.h
/// 
///  \brief Declaration of BaseNode, IProcessor
/// 
///  \version 1.0
/// 
///  \date    August/7/2013
//////////////////////////////////////////////////////////////////////////
#ifndef mcsf_log_iprocessor_h__
#define mcsf_log_iprocessor_h__
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <set>
#include <string>
#include <deque>
//#include <stdint>
//#include <fstream>
//#include <time.h>

#include "mcsf_log_common.h"
#include "mcsf_log_file_proxy.h"
#include "mcsf_logger_base.h"

#if defined(MCSF_PLATFORM_POSIX)
#include <linux/unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#elif defined(MCSF_PLATFORM_WIN32)
#include <Windows.h>
#endif

#if defined(MCSF_PLATFORM_WIN32)
#pragma warning(disable:4996)
#endif

NS_BEGIN

#if defined MCSF_PLATFORM_WIN32
#define UINT64_FMT "I64u"
#elif defined MCSF_PLATFORM_POSIX
#define UINT64_FMT "llu"
#endif

using namespace std;

class RawLog;

typedef boost::shared_array<char> CharArray;
class IProcessor;
typedef boost::shared_ptr<IProcessor> ProcessorPtr;
class ProcessorNode;
typedef boost::shared_ptr<ProcessorNode> ProcessorNodePtr;
class RawLog;
typedef boost::shared_ptr<RawLog> LogPtr;
#ifndef MCSF_PLATFORM_WIN32
    typedef unsigned int UINT32;
#endif

static const std::size_t LOG_Q_DEFAULT_MAX_SIZE = 100;
//static int g_iLostLogCount = 0;
static const UINT32 sg_carrThreshhold[2] = {20000, 100};

///\class	RawLog
///\brief	represent log data
class MCSF_LOG_API RawLog //: public ILog
{
public:
	CharArray _data;
	size_t _length;		
	~RawLog(){}
	CharArray Serialize()
	{
		return _data;
	}
};

///\Interface	IProcessor
///\brief		base class of Processor, which can process log data.
class MCSF_LOG_API IProcessor
{
public:
	virtual ~IProcessor(){}

	/// \brief		process log data from processor node queue
	/// \param		rawdata, pointer of log data from processor node queue
	/// \return		a log data pointer, the processed result.
	virtual LogPtr Process(const LogPtr& rawdata) = 0;

    /// \brief stop current process
    virtual void stop()
    {
    }

	/// \brief		clean data in processor's buffer
	/// \param		
	/// \return		a log data pointer, the processed result.
	virtual LogPtr Clean() 
	{
		return LogPtr();
	}

	virtual void Dump(FileProxyPtr file)
	{

	}

public:
	bool is_loop_runing;
};

///\class ProcessorNode
///\brief as a base class of nodes.
class MCSF_LOG_API ProcessorNode{
public:
    static std::string* GetClientName()
    {
        static std::string strClientName("");
        return &strClientName;
    }

    virtual ~ProcessorNode();
   /* ///\brief	construct a ProcessorNode object with a processor
    ///\param	processor, an object that running in ProcessorNode
    ///\param	have_timer_thread, indicate if continue processing on data queue is empty
    ///			if this param is true, ProcessorNode will call processor clean
    ///			every second when data queue is empty.
    ProcessorNode(ProcessorPtr processor, bool have_timer_thread);*/

    ///\brief	construct a ProcessorNode object with a processor
    ///\param	processor, an object that running in ProcessorNode
    ///\param	have_timer_thread, indicate if continue processing on data queue is empty
    ///			if this param is true, ProcessorNode will call processor clean
    ///			every second when data queue is empty.
    ProcessorNode(ProcessorPtr processor, std::size_t q_max_size, UINT32 iThreshhold, bool bIsTimerOn);

    ///\brief	make ProcessorNode enable
    ///\param	enable, bool value indicate if enable ProcessorNode.
    ///\return	void
    void Enable(const bool enable);

    ///\brief	get if ProcessorNode enabled
    ///\param	void
    ///\return	void
    bool Enable();

    ///\brief	set associated node
    ///\param	child, a ProcessorNode pointer of an associated node
    ///\return	void
    void SetChildNode(ProcessorNodePtr pNode)
    {
        m_pChildNode = pNode;
    }
    
    ///\brief	set associated node
    ///\param	child, a ProcessorNode pointer of an associated node
    ///\return	void
    ProcessorNodePtr GetChildNode()
    {
        return m_pChildNode;
    }

    ///\brief	push processed result to its children node
    ///\param	log, the processed result
    ///\param   bAllowToDrop, if allowed to drop current logs.
    ///\param   bForceSink, if force to sink to children
    ///\return	true for success, false for failure.
    bool DoSink(LogPtr log, bool bAllowToDrop, bool bForceSink);

   /* ///\brief check if log server is available
    ///\param VOID
    ///\return true for available, else for unavailable
    bool IsLogServerAvailable()
    {
        std::set<ProcessorNodePtr>::iterator it = _children.begin();
        while(it != _children.end())
        {
            if(0 == (*it)->GetName().compare("socket node"))
            {
                return (*it)->_processor.get()->get_connect_status();
            }
            ++it;
        }
        return false;
    }*/

    ///\brief	process thread function
    ///\param	void
    ///\return	void
    void loop();

    ///\brief	stop process thread
    ///\param	void
    ///\return	void
    void stop();

    ///\brief	start process thread
    ///\param	void
    ///\return	void
    void start();

    void dump(FileProxyPtr file);

    void flush();

    ProcessorPtr get_processor()
    {
        return _processor;
    }
protected:
    void clean();

    void PutSpecialLog();

    ///\brief	copy data pointer of the data queue to a tmp data queue
    ///\param	void
    ///\return	void
    //virtual void get_data_q() = 0;

   /* void swap_q_pointer()
    {
        boost::unique_lock<boost::mutex> lock(_mutex);
        deque<LogPtr>* tmp = _getting_q;
        _getting_q = _putting_q;
        _putting_q = tmp;
    }*/

    //virtual void SinkControl(LogPtr log);

    deque<LogPtr>				_dataq1;
    deque<LogPtr>				_dataq2;
    deque<LogPtr>*				_putting_q;
    deque<LogPtr>*				_getting_q;
    std::size_t					_q_max_size;
    ProcessorPtr				_processor;
    //std::set<ProcessorNodePtr>	_children;
    boost::mutex				_mutex;
    boost::condition_variable	_condition;
    boost::thread				_thread;
    bool						_enable;
    bool						_thread_running;
    bool						_have_timer_thread;
    UINT32                      _threshhold;

//     enum {flush_no, flush_go, flush_exchange1, flush_sent};
//     short                        _flushing_state;
//    bool                         _b_flush;

    int m_iLostLogCount;
    UINT32 _size;

private:
    ///\brief	clean data after process thread stopped
    ///\param	void
    ///\return	void

    bool swap_q_pointer();
    //std::string					_name;

    ///a pointer to an associated node
    ProcessorNodePtr m_pChildNode;

    //flush initial log entrys
    bool m_bInitialFlush;
};

class MCSF_LOG_API LogHelper{

public:
    static char *itoa_on_rtos(LogTypes::int64_t iNum,char *acString,int iRadix, int format_length = 0);
    
    static void ConstructUserDefinedLog(LogPtr& pLostLog);
};

NS_END

#endif // mcsf_log_iprocessor_h__


