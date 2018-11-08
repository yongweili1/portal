#ifndef HTTP_SERVER2_SERVER_HPP
#define HTTP_SERVER2_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "io_service_pool.hpp"
#include "connection_manager.hpp"
#include "callback_handle.hpp"

namespace netbasecore{
    /// The top-level class of the HTTP server.
    class server
        : private boost::noncopyable
    {
    public:
        /// Construct the server to listen on the specified TCP address and port, and
        /// serve up files from the given directory.
        explicit server(const std::string &port, 
            io_service_pool &pool, connection_manager &cm, callback_handle &callbak);


        std::string getMacPort();

        /// Handle a request to stop the server.
        void stop();

        std::string getIPsPort();

 
        static std::string getMacs();

        static std::string getIPInterface4();


    private:
        void check_deadline(connection_ptr pConnection);
        /// Initiate an asynchronous accept operation.
        void start_accept();

        /// Handle completion of an asynchronous accept operation.
        void handle_accept(connection_ptr pConnection, const boost::system::error_code& e);

        /// The pool of io_service objects used to perform asynchronous operations.
        io_service_pool & io_service_pool_;

        /// The signal_set is used to register for process termination notifications.

        /// Acceptor used to listen for incoming connections.
        boost::asio::ip::tcp::acceptor acceptor_;

        connection_manager &connection_manager_;

        callback_handle & callback_handlers_;
    };

}
#endif // HTTP_SERVER2_SERVER_HPP