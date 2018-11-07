#ifndef __CLIENT__
#define __CLIENT__

#include "connection_i.hpp"
#include "callback_handle.hpp"
#include "io_service_pool.hpp"
#include "connection_manager.hpp"
#include "mcsf_refined_log.h"

namespace netbasecore{

    class client : private boost::noncopyable
    {
    public:
        /// Constructor starts the asynchronous connect operation.
        client(io_service_pool &svr_pool, callback_handle &handlers, connection_manager& manager)
            : io_service_pool_(svr_pool),
            callback_handlers_(handlers),
            connection_manager_(manager),
            reconneting_(false),
            stopped_(false)
        {
        }

        bool check_cast_dispatcher(const std::string &addr, const std::string& port)
        {
            dispatcher_addr_ = addr;
            dispatcher_port_ = port;

            connection_ptr connDispatcher(new connection(
                io_service_pool_.get_io_service(),
                io_service_pool_.get_msg_handle_service(),
                io_service_pool_.get_reply_handle_service(), callback_handlers_, connection_manager_));

            if (!Connect(dispatcher_addr_, dispatcher_port_, 0, connDispatcher)) {
                return false;
            }

            return true;
        }

        std::string get_dispatcher_addr() const {
            return dispatcher_addr_ + ":" + dispatcher_port_;
        }

        bool Connect(const std::string &host,
            const std::string& port, unsigned int millisecTimeout, connection_ptr c = nullptr)
        {
            try{
                // Resolve the host name into an IP address.
                boost::asio::ip::tcp::resolver resolver(io_service_pool_.get_io_service());
                boost::asio::ip::tcp::resolver::query query(host, port);
                boost::asio::ip::tcp::resolver::iterator endpoint_iterator =
                    resolver.resolve(query);

                // Start an asynchronous connect operation.
                connection_ptr connection_;
                if (c) {
                    c->stop();
                    connection_ = c;
                }
                else
                {
                    connection_.reset(new connection(
                        io_service_pool_.get_io_service(),
                        io_service_pool_.get_msg_handle_service(),
                        io_service_pool_.get_reply_handle_service(), callback_handlers_, connection_manager_));
                }

                start_connect(connection_, endpoint_iterator);

                return connection_->connecting_wait(millisecTimeout);
            }
            catch (std::exception &e) {
                NTB_LOG_ERROR(e.what());
            }
        }

        void start_connect(connection_ptr connection, boost::asio::ip::tcp::resolver::iterator endpoint_iter)
        {
            if (endpoint_iter != boost::asio::ip::tcp::resolver::iterator())
            {
                boost::asio::async_connect(connection->socket(), endpoint_iter,
                    boost::bind(&client::handle_connect, this, connection, endpoint_iter,
                        boost::asio::placeholders::error));
            }
            else
            {
                connection->connecting_notify();
                connection->socket().close();
            }
        }

        /// Handle completion of a connect operation.
        void handle_connect(connection_ptr pConnection, 
            boost::asio::ip::tcp::resolver::iterator endpoint_iter, const boost::system::error_code& e)
        {
            if (!e)
            {
                // Successfully established connection. Start operation to read the list
                // of stocks. The connection::async_read() function will automatically
                // decode the data that is read from the underlying socket.
                pConnection->start();
            }
            else
            {
                NTB_LOG_INFO("Connect " << endpoint_iter->endpoint().address().to_string() << " error:" << e.message());

                pConnection->socket().close();

                start_connect(pConnection, ++endpoint_iter);
            }

        }

        void stop()
        {
            stopped_ = true;
            if (thread_reconnect_.joinable()) {
                thread_reconnect_.interrupt();
                thread_reconnect_.join();
            }
        }

        void loop_connect(connection_ptr c)
        {
            do {
                bool bConn = Connect(dispatcher_addr_, dispatcher_port_, 0, c);
                NTB_LOG_INFO(callback_handlers_.getMyName() <<
                    " reconnect dispatcher " << (bConn ? "successfully!" : "failed!"));
                if (bConn || stopped_)
                {
                    if (bConn && !stopped_ && register_handler_) {
                        if (!register_handler_()) {
                            //error call
                            NTB_LOG_WARN("re-register handler executed failed!");
                        }
                    }

                    reconneting_ = false;
                    break;
                }

                boost::this_thread::sleep(boost::posix_time::seconds(1));
            } while (!stopped_);
        }

        void handle_reconnect_dispatcher(connection_ptr c)
        {
            if (reconneting_)
                return;

            reconneting_ = true;

            thread_reconnect_ = boost::thread(boost::bind(&client::loop_connect, this, c));
        }

        void set_register_handler(boost::function<bool(void)> f) { register_handler_ = f; }

    private:
        io_service_pool & io_service_pool_;

        connection_manager &connection_manager_;

        callback_handle & callback_handlers_;

        bool reconneting_;
        bool stopped_;

        std::string dispatcher_addr_;
        std::string dispatcher_port_;

        boost::function<bool(void)> register_handler_;

        boost::thread thread_reconnect_;
    };
}
#endif
