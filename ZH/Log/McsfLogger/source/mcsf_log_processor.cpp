//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  xiaotao.yang (mailto:xiaotao.yang@united-imaging.com)
/// 
///  \file mcsf_log_processor.cpp
/// 
///  \brief ProcessorNode definition
/// 
///  \version 1.0
/// 
///  \date    August/7/2013
//////////////////////////////////////////////////////////////////////////
#include "../stdafx.h"
#include "mcsf_log_processor.h"
#include <iostream>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include "mcsf_logger_base.h"

#ifdef MCSF_PLATFORM_WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <stdio.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#endif

NS_BEGIN
using namespace std;

static std::string GetLocalIp()
{
    std::string strIp("local ip is ");
#ifdef WIN32
    WSADATA wsaData;
    int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (ret)
    {
        std::cerr << "fail to initialize wsa" << std::endl;
        return strIp + "unknown";
    }
    char host_name[256];
    if (SOCKET_ERROR == gethostname(host_name, sizeof(host_name)))
    {
        std::cerr << "get host name error!" << std::endl;
        return strIp + "unknown";
    }
    struct hostent *phe = gethostbyname(host_name);
    if (NULL == phe)
    {
        std::cerr << " get host by name error" << std::endl;
        return strIp + "unknown";
    }
    for (int i = 0; phe->h_addr_list[i] != 0; ++i)
    {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(addr));
        strIp += inet_ntoa(addr);
        strIp += "**";
    }
#else
    int sock;
    struct ifconf conf;
    struct ifreq *ifr;
    char buff[BUFSIZ];
    int num, i;
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (-1 == sock)
    {
        return strIp + "unknown";
    }
    conf.ifc_len = BUFSIZ;
    conf.ifc_buf = buff;
    ioctl(sock, SIOCGIFCONF, &conf);
    num = conf.ifc_len / sizeof(struct ifreq);
    ifr = conf.ifc_req;
    for (i = 0; i < num; i++)
    {
        struct sockaddr_in *sin = (struct sockaddr_in*)(&ifr->ifr_addr);
        ioctl(sock, SIOCGIFFLAGS, ifr);
        if (!(ifr->ifr_flags & IFF_LOOPBACK) && (ifr->ifr_flags & IFF_UP))
        {
            strIp += inet_ntoa(sin->sin_addr);
            strIp += "**";
        }
        ifr++;
    }
#endif
    return strIp;
}


ProcessorNode::ProcessorNode(ProcessorPtr processor, std::size_t q_max_size, UINT32 iThreshhold, bool bIsTimerOn)
    : _putting_q(&_dataq1)
    , _getting_q(&_dataq2)
    , _processor(processor)
    , _q_max_size(q_max_size)
    , _thread()
    , _enable(true)
    , _thread_running(true)
    , m_iLostLogCount(0)
    , m_pChildNode()
    , _threshhold(iThreshhold)
    , _size(0)
    , _have_timer_thread(bIsTimerOn)
    , m_bInitialFlush(true)
{
    if (q_max_size <= 0)
    {
        _q_max_size = LOG_Q_DEFAULT_MAX_SIZE;
    }
}

ProcessorNode::~ProcessorNode()
{
    if (_enable != false)
    {
        _thread_running = false;//TODO:_thread_running need sync???
        _condition.notify_one();
        if (_thread.joinable())
            _thread.timed_join(boost::posix_time::seconds(10));
    }

    try
    {
        ProcessorNodePtr pChild = GetChildNode();
        if (pChild)
            pChild->stop();
    }
    catch (std::exception& e)
    {
        std::cout << "file[" << __FILE__ << "] with line[" << __LINE__ << "]"
            "throw a exception with error msg:" << e.what() << "!" << std::endl;
    }
}

bool ProcessorNode::swap_q_pointer()
{
    bool bRet = false;
    boost::unique_lock<boost::mutex> lock(_mutex);
    if (_size < _q_max_size)
    {
        if (_have_timer_thread)
        {
            if (m_bInitialFlush)
            {
                _condition.timed_wait(lock
                    , boost::posix_time::milliseconds(10));
                if (_size > 0)
                {
                    bRet = true;
                    m_bInitialFlush = false;
                }
            }
            else
            {
                bool condition_ret = _condition.timed_wait(lock
                    , boost::posix_time::milliseconds(1000));
                bRet = !condition_ret;
            }
        }
        else
        {
            if (_thread_running)
            {
                //_condition.wait(lock);
				_condition.timed_wait(lock, boost::posix_time::minutes(10));
            }
        }
    }

    deque<LogPtr>* tmp = _getting_q;
    _getting_q = _putting_q;
    _putting_q = tmp;

    PutSpecialLog();
    _size = 0;

    return bRet;
}

bool ProcessorNode::DoSink(LogPtr log, bool bAllowToDrop, bool bForceSink)
{
    bool bRet = true;
    if (_thread_running)
    {
        boost::unique_lock<boost::mutex> lock(_mutex);
        if (_size < _threshhold || bForceSink)
        {
            _putting_q->push_back(log);
            ++_size;
        }
        else
        {
            if (bAllowToDrop)
            {
                _putting_q->push_back(log);
                _putting_q->pop_front();
                std::cout << "lost log" << std::endl;
                ++m_iLostLogCount;
            }
            bRet = false;
        }

        if (_size >= _q_max_size)
            _condition.notify_one();
    }
    return bRet;
}

void
ProcessorNode::Enable(const bool enable)
{
    if (_enable == enable)
    {
        return;
    }
    _enable = enable;
    if (enable)
    {
        stop();
    }
    else if (_thread == boost::thread())
    {
        start();
    }
}

bool
ProcessorNode::Enable()
{
    return _enable;
}

void
ProcessorNode::stop()
{
    _thread_running = false;//TODO:_thread_running need sync???
    _condition.notify_one();
    if (_thread.joinable())
        _thread.timed_join(boost::posix_time::microseconds(3000));
    _processor->stop();
    ProcessorNodePtr pChild = GetChildNode();
    if (pChild)
    {
        pChild->stop();
    }
    _enable = false;
}

void ProcessorNode::flush()
{
    //not implement
}

void
ProcessorNode::clean()
{
    boost::unique_lock<boost::mutex> lock(_mutex);
    deque<LogPtr>::iterator it = _putting_q->begin();
    ProcessorNodePtr pChildNode = GetChildNode();
    while (it != _putting_q->end())
    {
        LogPtr result = _processor->Process(*it);
        ++it;
        if (NULL != result && pChildNode)
        {
            pChildNode->DoSink(result, false, true);
        }
    }
    _putting_q->clear();

    LogPtr result = _processor->Clean();
    if (NULL != result && pChildNode)
    {
        pChildNode->DoSink(result, false, true);
    }
}

void ProcessorNode::start()
{
    ProcessorNodePtr pChild = GetChildNode();
    if (pChild)
        pChild->start();
    if (NULL == _processor)
        return;
    _thread_running = true;
    _thread = boost::thread(boost::bind(&ProcessorNode::loop, this));
}

void ProcessorNode::loop()
{
    if (NULL == _processor || (!_enable))
        return;

    while (_thread_running)
    {
        bool bNeedClearBuffer = swap_q_pointer();
        deque<LogPtr>::iterator it = _getting_q->begin();
        ProcessorNodePtr pChildNode = GetChildNode();

        while (it != _getting_q->end())
        {
            LogPtr result = _processor->Process(*it);
            ++it;
            if (NULL != result && pChildNode)
            {
                while (!pChildNode->DoSink(result, false, !_thread_running))
                {
#ifdef MCSF_PLATFORM_WIN32
                    Sleep(10); //for the performance, cannot sleep
#else
                    usleep(10000);
#endif
                }
            }
        }

        _getting_q->clear();

        if (bNeedClearBuffer)
        {
            LogPtr result = _processor->Clean();
            if (NULL != result && pChildNode)
            {
                pChildNode->DoSink(result, false, true);
            }
        }
    }
    clean();

    _processor->is_loop_runing = false;
}

void ProcessorNode::dump(FileProxyPtr file)
{
    _thread_running = false;
    deque<LogPtr>::iterator getting_it = _getting_q->begin();
    while (getting_it != _getting_q->end())
    {
        file->Write((*getting_it).get()->_data.get(), (*getting_it).get()->_length);
        ++getting_it;
    }
    deque<LogPtr>::iterator putting_it = _putting_q->begin();
    while (putting_it != _getting_q->end())
    {
        file->Write((*putting_it).get()->_data.get(), (*putting_it).get()->_length);
        ++putting_it;
    }

    ProcessorNodePtr pChildNode = GetChildNode();
    if (pChildNode)
        pChildNode->dump(file);

    this->_processor->Dump(file);
}

void ProcessorNode::PutSpecialLog()
{
    if (m_iLostLogCount == 0)
        return;
    LogPtr pLostLog;
    LogHelper::ConstructUserDefinedLog(pLostLog);
    size_t length = strlen("log items lost!");
    memcpy(pLostLog->_data.get() + pLostLog->_length, "log items lost!", length);
    pLostLog->_length += length;
    std::string strCount(" count = ");
    strCount += boost::lexical_cast<std::string>(m_iLostLogCount);
    length = strCount.size();
    memcpy(pLostLog->_data.get() + pLostLog->_length, strCount.c_str(), length);
    pLostLog->_length += length;
    std::string strIp = GetLocalIp();
    length = strIp.size();
    memcpy(pLostLog->_data.get() + pLostLog->_length, strIp.c_str(), length);
    pLostLog->_length += length;
    std::string strClientName(" logclientname: ");
    strClientName += *ProcessorNode::GetClientName();
    length = strClientName.size();
    memcpy(pLostLog->_data.get() + pLostLog->_length, strClientName.c_str(), length);
    pLostLog->_length += length;
    memcpy(pLostLog->_data.get() + pLostLog->_length, "\001\n", 2);
    pLostLog->_length += 2;
    _getting_q->push_front(pLostLog);
    m_iLostLogCount = 0;
}

void LogHelper::ConstructUserDefinedLog(LogPtr& pLostLog)
{
    static char stime[21] = { 0 };
    if (!pLostLog)
        pLostLog.reset(new RawLog());
    if (pLostLog)
    {
        pLostLog->_data = CharArray(new char[1024]);
		memset(pLostLog->_data.get(), 0, 1024);
        pLostLog->_length = 0;

        LogHelper::itoa_on_rtos(SocketSender::GetLogTimeU64(), stime, 10);

        std::string strFileName(__FILE__);
        sprintf(pLostLog->_data.get(), "LOG_DEV_INFO\002""%s""\002"
            "LogClient(%d:%d)\002""%s\002""%d\002""%s\002""0X0000000000000000\002",
            stime,
#ifdef MCSF_PLATFORM_WIN32
            GetCurrentProcessId()
#else
            getpid()
#endif
            ,
#ifdef MCSF_PLATFORM_WIN32
            GetCurrentThreadId()
#else
            (int)syscall(SYS_gettid)
#endif
            ,

            strFileName.substr(strFileName.find_last_of(
#ifdef MCSF_PLATFORM_WIN32
                '\\'
#else
                '/'
#endif
            ) + 1).c_str(),
            __LINE__,
            __FUNCTION__);
        pLostLog->_length = strlen(pLostLog->_data.get());
    }
}

char * LogHelper::itoa_on_rtos(LogTypes::int64_t iNum, char *acString, int iRadix, int format_length /*= 0*/ )
{
    typedef LogTypes::uint64_t UllInterger;
    char acIndex[] = "0123456789ABCDEF";
    UllInterger iUnsignedNum;
    int i = 0, j = 0, k = 0;
    char cTemp = '\0';
    if (iRadix == 10 && iNum < 0)
    {
        iUnsignedNum = (UllInterger)-iNum;
        acString[i++] = '-';
    }
    else
    {
        iUnsignedNum = (UllInterger)iNum;
    }
    do {
        acString[i++] = acIndex[iUnsignedNum % (UllInterger)iRadix];
        iUnsignedNum /= iRadix;
    } while (iUnsignedNum);
    acString[i] = '\0';
    if (acString[0] == '-')
    {
        k = 1;
    }
    else
    {
        k = 0;
        i--;
    }
    for (j = k; j <= (i - k) / 2.0; j++)
    {
        cTemp = acString[j];
        acString[j] = acString[i - j];
        acString[i - j] = cTemp;
    }
	if (k == 0 && format_length != 0)
	{
		j = format_length - i - 1;
		while (i >= 0)
		{
			acString[i + j] = acString[i];
			i--;
		}
		acString[format_length] = '\0';
		memset(acString, (20 == format_length) ? ' ' : '0', j);
	}
    return acString;
}

NS_END
