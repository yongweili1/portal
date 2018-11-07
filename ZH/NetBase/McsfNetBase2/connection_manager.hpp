#ifndef HTTP_CONNECTION_MANAGER_HPP
#define HTTP_CONNECTION_MANAGER_HPP

#include <set>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include "connection_i.hpp"

namespace netbasecore{

        /// Manages open connections so that they may be cleanly stopped when the server
        /// needs to shut down.
        class connection_manager
            : private boost::noncopyable
        {
        public:
            connection_manager() :reconnect_handler_(0), disconnect_handler(0) {}

            void set_reconnect_handler(boost::function<void(connection_ptr)> h) {
                reconnect_handler_ = h;
            }

            void set_disconnect_handler(boost::function<void(const std::string& )> f)
            {
                disconnect_handler = f;
            }

            /// Stop the specified connection.
            void handle_connection(connection_ptr c);

            /// Stop all connections.
            void stop_all();

            connection_ptr get_connection(const std::string &name);

            std::vector<connection_ptr> get_online();

        private:
            /// The managed connections.
            std::set<connection_ptr> connections_;
            boost::mutex mx_;
            boost::function<void(connection_ptr)> reconnect_handler_;
            boost::function<void(const std::string &)> disconnect_handler;
        };

    } // namespace server


#endif // HTTP_CONNECTION_MANAGER_HPP