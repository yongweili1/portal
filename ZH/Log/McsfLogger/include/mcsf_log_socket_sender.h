//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfLogClient
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2011
///  All rights reserved.
///  \author        lei.sun 
///  \email         mailto:lei.sun@united-imaging.com
///  \file          mcsf_log_xml_parse.h
///  \brief         parse the log client xml configuration file
///  \date          July 2 , 2012
///  \commented by  xiaotao yang
///
//////////////////////////////////////////////////////////////////////////
#ifndef mcsf_log_socket_sender_h__
#define mcsf_log_socket_sender_h__

#include "mcsf_log_processor.h"
#include "mcsf_logger_common.h"
#include <fstream>
#include <boost/asio.hpp>

NS_BEGIN

using namespace std;
using namespace boost::asio::ip;

extern std::string g_sLocalLogPath;

///brief: derive a processor class from IProcessor, responsible for sending logs
class MCSF_LOGGER_API SocketSender : public IProcessor
{
public:

    static boost::uint64_t GetLogTimeU64 ();
	/// brief: constructor 
	/// param: 
	///        [in] string server_addr: server ip address
	///        [in] string server_port: server port number in string
	///        [in] unsigned short server_port: server port number in short
	SocketSender(const string &server_addr, const string &server_port);

    bool get_connect_status();

    void stop();

	///brief : deconstructor
	~SocketSender();
public:
	///brief: processor function responsible for sending logs(by call send) 
	/// param:
	///          [in] const LogPtr& rawdata: log data to be sent
	/// retval:
	///          must be LogPtr() for SocketSender processor
	virtual LogPtr Process(const LogPtr& rawdata);	

private:
	///brief: direct log sending function
	/// param:
	///          [in] CharArray log: pointer to buffer to be sended
	///          [in] std::size_t length: buffer size
	///retval:
	///           void
	bool send(CharArray log, std::size_t length);

	///brief: launch connecting to log server
	///param: void
	///retval: void
	void start();

    bool SendClientTimeStamp();

    boost::system::error_code sync_send_buffer(CharArray log, std::size_t length, boost::posix_time::time_duration timeout);

    void error_processor();

	void sendClientTimeSyncMsg();

    void check_deadline();

    void loopConnect();

    bool sendFirstLogItem();

    bool writeToLocalFile(CharArray log, std::size_t length);

public:
     bool _stop_flag;
private:
	boost::thread           _thread;
	boost::asio::io_service _service;
	std::string				_server;
	std::string         	_port;
	bool					_connected;
	boost::shared_ptr<tcp::socket> _socket;
	//tcp::socket				_socket;
	//ofstream					_file_stream;
   // boost::mutex            _sent_mx;
	//the name of local buffer file, saving logs while unconnected from server
	std::string				_buffer_file_name;
	// buffering flag
	boost::uint64_t         _lastSyncTime;

    boost::asio::deadline_timer _deadline;
    bool                    _reconnecting;
};

NS_END


#endif // mcsf_log_socket_sender_h__