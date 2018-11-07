#include "connection_manager.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include "mcsf_refined_log.h"


#include "id_def.h"

namespace netbasecore{

    void connection_manager::handle_connection(connection_ptr c)
    {
        boost::mutex::scoped_lock lk(mx_);

        if (connection::connection_connected == c->connection_stat())
        {
            connections_.insert(c);
        }
        else if (connection::connection_name_get == c->connection_stat())
        {
            c->connecting_notify();
        }
        else if (connection::connection_disconnected == c->connection_stat())
        {
            NTB_LOG_INFO("disconnected with " << c->peer_name());
            connections_.erase(c);
            c->stop();

            if (disconnect_handler) {
                disconnect_handler(c->peer_name());
            }

            if (c->peer_name() == dispatcher_name) {
                if (reconnect_handler_) {
                    reconnect_handler_(c);
                }
            }
        } 
        else {}
    }

    void connection_manager::stop_all()
    {
        boost::mutex::scoped_lock lk(mx_);
        std::for_each(connections_.begin(), connections_.end(),
            boost::bind(&connection::stop, _1));
        connections_.clear();
    }

    connection_ptr connection_manager::get_connection(const std::string &name)
    {
        boost::mutex::scoped_lock lk(mx_);
        BOOST_FOREACH(connection_ptr p, connections_) {
            if (p->peer_name() == name) {
                return p;
            }
        }
        return nullptr;
    }

    std::vector<connection_ptr> connection_manager::get_online()
    {
        boost::mutex::scoped_lock lk(mx_);
        std::vector<connection_ptr> tmp;
        BOOST_FOREACH(connection_ptr p, connections_) {
            if (connection::connection_name_get == p->connection_stat()) {
                tmp.push_back(p);
            }
        }
        return tmp;
    }

}
