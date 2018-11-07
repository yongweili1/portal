#ifndef mcsf_log_socket_receiver_h__
#define mcsf_log_socket_receiver_h__
#include "mcsf_log_connection.h"
#include "mcsf_log_connection_manager.h"

NS_BEGIN

class SocketReceiver: private boost::noncopyable
{
public:
	explicit SocketReceiver(const std::string& address
		, const std::string& port);
	void Run();
	void Stop();

private:
	void handle_accept(const boost::system::error_code& e);
	void handle_stop();
	boost::asio::io_service _io_service;
	boost::asio::ip::tcp::acceptor _acceptor;
	connection_manager _connection_manager;
	connection_ptr _new_connection;
};

typedef SocketReceiver LogServer;

NS_END
#endif // mcsf_log_receiver_h__
