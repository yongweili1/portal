#ifndef MCSF_LOG_CONNECTION_HPP
#define MCSF_LOG_CONNECTION_HPP
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>
#include "mcsf_log_server_buffer.h"
NS_BEGIN

class connection_manager;

class connection
	: public boost::enable_shared_from_this<connection>
	, private boost::noncopyable
{
public:
	explicit connection(boost::asio::io_service& io_service
		, connection_manager& manager);

	boost::asio::ip::tcp::socket& socket();

	void start();

	void stop();

	~connection();

private:
    void write_connect_info(const char *pClientConnectStatus);

	void handle_read_content(const boost::system::error_code& e
		, std::size_t bytes_transferred);

	void handle_read_TimeStamp(boost::shared_array<char> pHeader
        , std::size_t headerLen
        , const boost::system::error_code& e
		, std::size_t bytes_transferred);

    boost::uint64_t GetCurrentTimeStamp64();
	void GetServerIp();

private:
	boost::asio::ip::tcp::socket	_socket;
	connection_manager&				_connection_manager;
	log_server_buffer               _buffer;
    uint64_t                        m_iDeltaTime;
	std::vector<std::string> m_ipvec;
	std::string m_client_ip;
};

typedef boost::shared_ptr<connection> connection_ptr;

NS_END

#endif //MCSF_LOG_CONNECTION_HPP
