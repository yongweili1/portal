#include "../stdafx.h"
#include "mcsf_log_socket_sender.h"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> 
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include "boost/regex.hpp"
#include <stdio.h>


NS_BEGIN
std::string g_sLocalLogPath = "./log/";
static const std::size_t TMP_BUF_LEN = 65536;
static const int TRY_CONNECT_MAX_COUNT = 10;
const uint64_t time_shift = 116444736000000000ULL; // (27111902 << 32) + 3577643008
char STAMP_HEADER[] = { 0x0E, 0x0F };


SocketSender::SocketSender(const string &server_addr, const string &server_port)
    : _socket(new tcp::socket(_service))
    , _server(server_addr)
    , _port(server_port)
    , _connected(false)
    , _lastSyncTime(0)
    , _buffer_file_name(boost::posix_time::to_iso_string(
        boost::posix_time::microsec_clock::universal_time()) + "-" +
        boost::lexical_cast<std::string>(
#ifndef MCSF_PLATFORM_WIN32
            getpid()
#else
            GetCurrentProcessId()
#endif
            )
        + ".uihlog")
    , _stop_flag(false)
    , _deadline(_service)
    , _reconnecting(false)
{
    start();
    is_loop_runing = true;
    _deadline.expires_at(boost::posix_time::pos_infin);
    check_deadline();
}

SocketSender::~SocketSender()
{
    stop();
}

void SocketSender::loopConnect()
{
    try {
        tcp::resolver::iterator itr =
            tcp::resolver(_service).resolve(tcp::resolver::query(_server, _port));

        boost::system::error_code c;
        while (!_connected && !_stop_flag)
        {
            for (tcp::resolver::iterator i = itr; i != tcp::resolver::iterator(); ++i)
            {
                _socket->connect(*i, c);
                printf("[log]connecting %s:%d...message=%s\n",
                    i->endpoint().address().to_string().c_str(),
                    i->endpoint().port(), c.message().c_str());
                if (!c && _socket->is_open())
                {
                    printf("[log]connect server success.\n");
                    if (SendClientTimeStamp() && sendFirstLogItem())
                    {
                        _connected = true;
                        break;
                    }

                }
            }

            if (_connected)
                break;

            boost::this_thread::sleep(boost::posix_time::seconds(1));
        }

        _reconnecting = false;
    }
    catch (std::exception & ex)
    {
        printf("loopConnect:%s\n", ex.what());
    }
}

bool SocketSender::sendFirstLogItem()
{
    LogPtr pLog;
    LogHelper::ConstructUserDefinedLog(pLog);
    std::string strIp("IP address:");
    strIp += _socket->local_endpoint().address().to_string();
    strIp += ", Port:";
    strIp += boost::lexical_cast<std::string>(_socket->local_endpoint().port());
    strIp += " log client name = ";
    strIp += *ProcessorNode::GetClientName();
    strIp += "\001\n";
    memcpy(pLog->_data.get() + pLog->_length, strIp.c_str(), strIp.size());
    pLog->_length += strIp.size();

    return 0 == sync_send_buffer(pLog->_data, pLog->_length, boost::posix_time::seconds(5));
}

LogPtr
SocketSender::Process(const LogPtr& rawdata)
{
    if (NULL != rawdata)
    {
        while (!send(rawdata->_data, rawdata->_length) && !_stop_flag)
        {
#ifdef MCSF_PLATFORM_WIN32
            Sleep(5);
#else
            usleep(5000);
#endif
        }
    }
    return LogPtr();
}

void
SocketSender::start()
{
    if (_connected || _reconnecting)
        return;

    _reconnecting = true;

    _thread = boost::thread(boost::bind(&SocketSender::loopConnect, this));
}

void
SocketSender::stop()
{
    _stop_flag = true;

    boost::system::error_code ec;
    _socket->close(ec);

    if (_thread.joinable())
        _thread.timed_join(boost::posix_time::seconds(1));
}

bool SocketSender::get_connect_status()
{
    return _connected;
}


boost::system::error_code SocketSender::sync_send_buffer(CharArray log, std::size_t length, boost::posix_time::time_duration timeout)
{
    _deadline.expires_from_now(timeout);
    boost::system::error_code ec = boost::asio::error::would_block;

    boost::asio::async_write(*_socket
        , boost::asio::buffer(log.get(), length)
        , boost::asio::transfer_all()
        , boost::lambda::var(ec) = boost::lambda::_1);

    // Block until the asynchronous operation has completed.
    boost::system::error_code ecRun;
    do {
        _service.run_one(ecRun);
        if (ecRun) return ecRun;
    } while (ec == boost::asio::error::would_block);

    return ec;
}

bool SocketSender::SendClientTimeStamp()
{
    boost::uint64_t tm = SocketSender::GetLogTimeU64();

    size_t szHeaderLen = sizeof(STAMP_HEADER);
    size_t szTmLen = sizeof(tm);
    CharArray pbuff(new char[szHeaderLen + szTmLen]);
    memcpy(pbuff.get(), STAMP_HEADER, szHeaderLen);
    memcpy(pbuff.get() + szHeaderLen, &tm, sizeof(tm));

    boost::system::error_code err = sync_send_buffer(pbuff, szHeaderLen + szTmLen, boost::posix_time::seconds(5));
    return 0 == err;
}

void SocketSender::error_processor()
{
    _connected = false;

    boost::system::error_code ec;
    _socket->close(ec);

    _service.reset();

    start();
}

bool SocketSender::writeToLocalFile(CharArray log, std::size_t length)
{
    try
    {
        boost::filesystem::path pathname(g_sLocalLogPath);
        if (!boost::filesystem::exists(pathname) || !boost::filesystem::is_directory(pathname))
        {
            boost::filesystem::create_directories(pathname);
#ifdef MCSF_PLATFORM_LINUX
            system(("chmod 777 " + pathname.string() + (" -R")).c_str());
#endif
        }
        pathname /= _buffer_file_name;

        fstream file_stream(pathname.string().c_str(),
            std::fstream::app | std::fstream::binary | std::fstream::in | std::fstream::out);
        if (file_stream.is_open()) {
            file_stream.seekp(0, std::ios_base::end);
            size_t fileSize = file_stream.tellp();
            if (fileSize == 0) {
                std::streamsize iOffset = sizeof(iOffset);
                file_stream.write(reinterpret_cast<const char *>(&iOffset), sizeof(iOffset));
            }
            file_stream.write(log.get(), length);
            file_stream.close();
            return true;
        }
        else {
            return false;
        }
    }
    catch (...)
    {
        return false;
    }
    
}

bool SocketSender::send(CharArray log, std::size_t length)
{
    if (_connected)
    {
        boost::uint64_t tm = GetLogTimeU64();
        if (tm - _lastSyncTime > 300000) //5 mins
        {
            sendClientTimeSyncMsg();
            _lastSyncTime = tm;
        }

        if (length > 0 && log != NULL)
        {
            boost::system::error_code err = sync_send_buffer(log, length, boost::posix_time::seconds(10));
            if (err)
            {
                // 如果出错，调用处保证了会再次调用该函数,将log block写到本地文件
                printf("[log]disconnect from server. reconnecting...\n");
                error_processor();
                return false;
            }
        }
        return true;
    }
    else
    {
        if (!_reconnecting)
        {
            start();
        }

        return writeToLocalFile(log, length);
    }
}

boost::uint64_t SocketSender::GetLogTimeU64()
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

void SocketSender::sendClientTimeSyncMsg()
{
    LogPtr pLog;
    LogHelper::ConstructUserDefinedLog(pLog);
    std::string str("This Message is used for Client Time Sync");
    str += "\001\n";
    memcpy(pLog->_data.get() + pLog->_length, str.c_str(), str.size());
    pLog->_length += str.size();
    boost::regex reg("LOG_DEV_INFO(.*)$");
    boost::cmatch m;
    if (boost::regex_match(pLog->_data.get(), reg))
    {
        if (boost::regex_search(pLog->_data.get(), m, reg))
        {
            std::string tmpData = boost::regex_replace(std::string(pLog->_data.get()), reg, "LOG_TIME_SYNC""$1");
            pLog->_data.reset(new char[strlen(tmpData.c_str()) + 1]);
            strcpy(pLog->_data.get(), tmpData.c_str());
            pLog->_length += 1;
        }
    }

    boost::system::error_code err = sync_send_buffer(pLog->_data, pLog->_length, boost::posix_time::seconds(5));
    if (err)
    {
        printf("[log]disconnect from server. reconnecting...\n");
        error_processor();
        return;
    }
}

void SocketSender::check_deadline()
{
    // Check whether the deadline has passed. We compare the deadline against
    // the current time since a new asynchronous operation may have moved the
    // deadline before this actor had a chance to run.
    if (_deadline.expires_at() <= boost::asio::deadline_timer::traits_type::now())
    {
        if (_connected) {
            printf("[log] send server timeout! reconnecting...\n");
            error_processor();
        }
        // There is no longer an active deadline. The expiry is set to positive
        // infinity so that the actor takes no action until a new deadline is set.
        _deadline.expires_at(boost::posix_time::pos_infin);
    }

    //printf("%s, %d\n", boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time()).c_str());
    // Put the actor back to sleep.
    _deadline.async_wait(boost::bind(&SocketSender::check_deadline, this));
}

NS_END
