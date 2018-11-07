#include "stdafx.h"
#include "mcsf_log_connection.h"

#include <boost/bind.hpp>
#include "mcsf_log_connection_manager.h"
#include <stdio.h>
#include <winsock2.h>
//test
#include <boost/timer.hpp>
#include "mcsf_log_file.h"
//endtest
using namespace std;
NS_BEGIN

#define UINT64_FMT "I64u"
static const boost::uint64_t time_shift = 116444736000000000ULL; // (27111902 << 32) + 3577643008
char STAMP_HEADER[] = {0x0E, 0x0F};

connection::connection(boost::asio::io_service& io_service
	, connection_manager& manager)
	: _socket(io_service)
	, _connection_manager(manager)
    , m_iDeltaTime(0)
{
		GetServerIp();
}

boost::asio::ip::tcp::socket& connection::socket()
{
	return _socket;
}

connection::~connection()
{

}

void connection::write_connect_info(const char *pClientConnectStatus)
{
    try{
        std::size_t pos(0), length(strlen("LOG_DEV_INFO\002"));
        memcpy(_buffer.getp() + pos, "LOG_DEV_INFO\002", length);
        pos += length;
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);	
        union {
	        FILETIME as_file_time;
	        boost::uint64_t as_integer; // 100-nanos since 1601-Jan-01
        } time_caster;
        time_caster.as_file_time = ft;
        time_caster.as_integer -= time_shift; // filetime is now 100-nanos since 1970-Jan-01
        sprintf(_buffer.getp() + pos, "%13"UINT64_FMT"\002", time_caster.as_integer / 10000UL);
        pos += 14;
        std::string src("LogServer(");
        src += boost::lexical_cast<std::string>(GetCurrentProcessId()) + ":" +
            boost::lexical_cast<std::string>(GetCurrentThreadId()) + ")\002";
        length = src.length();
        memcpy(_buffer.getp() + pos, src.c_str(), length);
        pos += length;
        char caFile[256];
        strcpy(caFile, __FILE__);
        length = strlen(caFile);
        while(length)
        {
            length --;
            if(caFile[length] == '/' || caFile[length] == '\\')
            {
                length += 1;
                break;
            }
        }
        std::size_t tmp(strlen(&caFile[length]));
        memcpy(_buffer.getp() + pos, &caFile[length], tmp);
        pos += tmp;
        memcpy(_buffer.getp() + pos, "\002", 1);
        pos += 1;
        char strLineNo[7];
        sprintf(strLineNo, "%d\002", __LINE__);
        length = strlen(strLineNo);
        memcpy(_buffer.getp() + pos, strLineNo, length);
        pos += length;
        length = strlen(__FUNCTION__);
        memcpy(_buffer.getp() + pos, __FUNCTION__, length);
        pos += length;
        memcpy(_buffer.getp() + pos, "\002", 1);
        pos += 1;
        memcpy(_buffer.getp() + pos, "0X0000000000000000\002", 19);
        pos += 19;
        length = strlen(pClientConnectStatus);
        memcpy(_buffer.getp() + pos, pClientConnectStatus, length);
        pos += length;
        std::string strIp("ip address: ");
        strIp += _socket.remote_endpoint().address().to_string();
        strIp += ", port: ";
        strIp += boost::lexical_cast<std::string>(_socket.remote_endpoint().port());
        strIp += "\001\n";
        length = strIp.length();
        memcpy(_buffer.getp() + pos, strIp.c_str(), length);
        pos += length;
        _buffer.write(pos, m_iDeltaTime);
    }
    catch(std::exception &e)
    {
        std::cerr << "error in func[" << __FUNCTION__ << "]!with msg" << e.what() << std::endl;
    }
}

void connection::start()
{
	try
	{
		//for log file
		//_buffer.reset(new char[FIXED_BUFFER_LEN]);
        if(!_buffer.getp())
        {
            std::cout << "start connection failed!!" << std::endl;
            write_connect_info("one client disconnect to server!");
            _connection_manager.stop(shared_from_this());
            return;
        }
        write_connect_info("one client connects to server!");
		m_client_ip = _socket.remote_endpoint().address().to_string();

        std::size_t stampHeaderLen = sizeof(STAMP_HEADER) + sizeof(uint64_t);
        boost::shared_array<char> pStampHeader(new char[stampHeaderLen]);
		_socket.async_read_some(
            boost::asio::buffer(pStampHeader.get(), stampHeaderLen)
			, boost::bind(&connection::handle_read_TimeStamp
                , shared_from_this()
                , pStampHeader
                , stampHeaderLen
			    , boost::asio::placeholders::error
			    , boost::asio::placeholders::bytes_transferred));
	}
	catch(...)
	{
        write_connect_info("one client disconnect to server!");
		_connection_manager.stop(shared_from_this());
	}
}

void connection::stop()
{
	_socket.close();
}

void connection::handle_read_TimeStamp(boost::shared_array<char> pHeader
    , std::size_t headerLen
    , const boost::system::error_code& e 
    , std::size_t bytes_transferred )
{
    if(e)
    {
        write_connect_info("one client disconnect to server!");
        _connection_manager.stop(shared_from_this());
    }
    else
    {
        if (0==memcmp(STAMP_HEADER, pHeader.get(), sizeof(STAMP_HEADER))
            && headerLen==bytes_transferred)
        {
			if (find(m_ipvec.begin(),m_ipvec.end(),m_client_ip)==m_ipvec.end())
			{
				boost::uint64_t tm=0;
				memcpy(&tm, pHeader.get()+sizeof(STAMP_HEADER), sizeof(tm));
				m_iDeltaTime = GetCurrentTimeStamp64() - tm;


			}
        }
        else
        {
            memcpy(_buffer.getp(), pHeader.get(), bytes_transferred);
            _buffer.write(bytes_transferred, m_iDeltaTime);
        }

        _socket.async_read_some(boost::asio::buffer(_buffer.getp()
            , _buffer.psize())
            , boost::bind(&connection::handle_read_content
            , shared_from_this()
            , boost::asio::placeholders::error
            , boost::asio::placeholders::bytes_transferred));
    }
}


void connection::handle_read_content(const boost::system::error_code& e
	, std::size_t length)
{
	if(e)
	{
        write_connect_info("one client disconnect to server!");
		_connection_manager.stop(shared_from_this());
	}
	else
	{
		_buffer.write(length, m_iDeltaTime);
		try
		{
			_socket.async_read_some(boost::asio::buffer(_buffer.getp()
				, _buffer.psize())
				, boost::bind(&connection::handle_read_content
				, shared_from_this()
				, boost::asio::placeholders::error
				, boost::asio::placeholders::bytes_transferred));

		}
		catch(...)
		{
            write_connect_info("one client disconnect to server!");
			_connection_manager.stop(shared_from_this());
		}
	} 
}


void connection::GetServerIp()
{
	try
	{
		m_ipvec.clear();
		m_ipvec.push_back("127.0.0.1");
		WSADATA sData; 
		WSAStartup(MAKEWORD(2, 2), &sData); 
		char host_name[255];
		//获取本地主机名称
		if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR) 
		{
			printf("Error %d when getting local host name.", WSAGetLastError());
		}
		//从主机名数据库中得到对应的“主机”
		struct hostent *phe = gethostbyname(host_name);
		//循环得出本地机器所有IP地址
		for (int i = 0; phe->h_addr_list[i] != 0; ++i)
		{
			struct in_addr addr;
			memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
			m_ipvec.push_back(inet_ntoa(addr));
		}
		WSACleanup(); 
	}
	catch (...)
	{
		printf("Failed to get server IP!");
	}
	

}




boost::uint64_t connection::GetCurrentTimeStamp64()
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
