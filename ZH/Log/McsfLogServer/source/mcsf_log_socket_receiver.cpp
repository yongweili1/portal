#include "stdafx.h"
#include "mcsf_log_socket_receiver.h"
#include <boost/thread.hpp>
NS_BEGIN

using namespace std;
using namespace boost::asio::ip;

SocketReceiver::SocketReceiver(const std::string& sAddress
	, const std::string& sPort)
	: _io_service()
	, _acceptor(_io_service)
	, _connection_manager()
	, _new_connection(new connection(_io_service
	, _connection_manager))
{
    try
    {
        unsigned short port = boost::lexical_cast<unsigned short>(sPort);
        address addr = address::from_string(sAddress);
        tcp::endpoint endpoint(addr, port);

        _acceptor.open(endpoint.protocol());
        _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        _acceptor.bind(endpoint);
        _acceptor.listen();
        std::cout<<"log server addr and port is " 
            << endpoint.address().to_string()
            << " : " 
            << endpoint.port() << std::endl;

        _acceptor.async_accept(_new_connection->socket()
            , boost::bind(&SocketReceiver::handle_accept
            , this
            , boost::asio::placeholders::error));
    }
    catch (std::exception& ex)
    {
        std::cout <<"func[SocketReceiver::SocketReceiver()] with exception:" << ex.what() << std::endl;
    }
}

void 
SocketReceiver::Run()
{
   /* std::cout << "before current thread priority = " << GetThreadPriority(GetCurrentThread()) << std::endl;
    if(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL))
    {
        std::cout << "after current thread priority = " << GetThreadPriority(GetCurrentThread()) << std::endl;
    }*/
	_io_service.run();
	//std::cout<<"ioserver run end"<<std::endl;
}

void 
SocketReceiver::Stop()
{
	_io_service.post(boost::bind(&SocketReceiver::handle_stop, this));
}

void 
SocketReceiver::handle_accept(const boost::system::error_code& e)
{
	if (e)
	{
		std::cout << "accept error! message = " << e.message() << std::endl;
	}
	else
	{
		_connection_manager.start(_new_connection);
		std::cout<<"accepted addr = " 
			<< _new_connection->socket().remote_endpoint().address().to_string()
			<< ":" 
			<< _new_connection->socket().remote_endpoint().port()
			<< std::endl;
		_new_connection.reset(new connection(_io_service
			, _connection_manager));
		_acceptor.async_accept(_new_connection->socket()
			, boost::bind(&SocketReceiver::handle_accept
			, this
			, boost::asio::placeholders::error));
	}
}

void 
SocketReceiver::handle_stop()
{
	_acceptor.cancel();
	_acceptor.close();
	_connection_manager.stop_all();
	_io_service.stop();
}

NS_END