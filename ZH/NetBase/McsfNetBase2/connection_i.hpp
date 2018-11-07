
#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "rpc.h"

namespace netbasecore {

    
    class connection;
    typedef boost::shared_ptr<connection> connection_ptr;

    class callback_handle;
    class connection_manager;
    /// Represents a single connection from a client.
    class connection
        : public boost::enable_shared_from_this<connection>,
        private boost::noncopyable
    {
    public:
        enum {connection_disconnected, connection_connected, connection_name_get};

        /// Construct a connection with the given io_service.
        explicit connection(boost::asio::io_service& io_service,
            boost::asio::io_service& msg_handle_service,
            boost::asio::io_service& reply_handle_srv,
            callback_handle & call_handler,
            connection_manager &connection_mng);


        /// Get the socket associated with the connection.
        boost::asio::ip::tcp::socket& socket();

        /// Start the first asynchronous operation for the connection.
        void start();

        /// Stop all asynchronous operations associated with the connection.
        void stop();

        bool push_message(util::RpcMsgPackagePtr p);

        std::string peer_name() { return peer_name_; }

        int connection_stat() { return connected_stat_; }

        std::string get_remote_addr() { return remote_addr_ ; }

        //for client
        bool connecting_wait(unsigned int millisec);

        void connecting_notify();

    private:

        void handle_disconnection();

        void do_async_write();

        void handle_read_head(const boost::system::error_code& e, std::size_t bytes_transferred, util::RpcMsgPackagePtr p);

        void handle_read_body(const boost::system::error_code& e, std::size_t bytes_transferred, util::RpcMsgPackagePtr p);

        /// Handle completion of a write operation.
        void handle_write(const boost::system::error_code& e, util::RpcMsgPackagePtr p);

        void RpcMsgPostHandler(const util::RpcMsgPackagePtr p);

        void check_connection_timer();


    private:

        /// Socket for the connection.
        boost::asio::ip::tcp::socket socket_;

        boost::asio::io_service& msg_handle_service_;

        boost::asio::io_service& reply_handle_service_;

        boost::mutex list_mutex_;

        std::list<util::RpcMsgPackagePtr > list_output_;

        bool processing_;

        int connected_stat_;

        std::string peer_name_;
        std::string remote_addr_;

        callback_handle &callback_handler_;

        connection_manager &connection_mng_;

        boost::mutex connectting_mx_;
        boost::condition_variable_any connectting_condv_;
        boost::asio::deadline_timer deadline_;
        bool stopped_;

        boost::posix_time::ptime heartbeat_pt_;

    };
} // namespace server


#endif // HTTP_CONNECTION_HPP