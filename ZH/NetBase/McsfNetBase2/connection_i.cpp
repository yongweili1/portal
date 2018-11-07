#include <boost/assert.hpp>

#include "mcsf_refined_log.h"
#include "connection_i.hpp"
#include "connection_manager.hpp"
#include "callback_handle.hpp"

namespace netbasecore{

    connection::connection(boost::asio::io_service& io_service,
        boost::asio::io_service& msg_handle_service,
        boost::asio::io_service& reply_handle_srv,
        callback_handle & call_handler,
        connection_manager &connection_mng)
        : socket_(io_service),
        msg_handle_service_(msg_handle_service),
        reply_handle_service_(reply_handle_srv),
        callback_handler_(call_handler),
        connection_mng_(connection_mng),
        processing_(false),
        connected_stat_(connection_disconnected),
        stopped_(false),
        deadline_(io_service),
        heartbeat_pt_(boost::date_time::pos_infin)
    {

    }
    
    boost::asio::ip::tcp::socket& connection::socket()
    {
        return socket_;
    }

    void connection::RpcMsgPostHandler(const util::RpcMsgPackagePtr p)
    {
        if (p->m_header.iMsgType == util::msg_type_first)
        {
            peer_name_ = p->m_sData;
            NTB_LOG_INFO(peer_name_ << "<-peer [connection build] me->" << callback_handler_.getMyName());

            {
                boost::mutex::scoped_lock lck(connectting_mx_); 
                connected_stat_ = connection_name_get;
            }
            connection_mng_.handle_connection(shared_from_this());
        }
        else if (p->m_header.iMsgType == util::msg_type_cmd_reply)
        {
            p->setSender(peer_name_);
            callback_handler_.handle_cmd_reply_no_block(p, shared_from_this(), reply_handle_service_);
        }
        else if (p->m_header.iMsgType == util::msg_type_heartbeat) {
        }
        else
        {
            p->setSender(peer_name_);
             msg_handle_service_.post(boost::bind(&callback_handle::handle_message,
                     boost::ref(callback_handler_), p, shared_from_this()));
        }

        heartbeat_pt_ = boost::posix_time::second_clock::universal_time();
    }

    bool connection::push_message(util::RpcMsgPackagePtr p)
    {
        if (connected_stat_ == connection_name_get)
        {
            boost::mutex::scoped_lock l(list_mutex_);
            list_output_.push_back(p);
        }
        else {
            return false;
        }

        if (!processing_)
            do_async_write();

        return true;
    }

    bool connection::connecting_wait(unsigned int millisec)
    {
        boost::mutex::scoped_lock lck(connectting_mx_);

        if (connected_stat_ == connection_name_get) { 
            return true;
        }
        else
        {
            if (millisec)
                return connectting_condv_.timed_wait(lck, boost::posix_time::millisec(millisec));
            else
            {
                connectting_condv_.wait(lck);
            }
        }
        return connected_stat_ == connection_name_get;
    }

    void connection::connecting_notify()
    {
        boost::mutex::scoped_lock lck(connectting_mx_);
        connectting_condv_.notify_one();
    }

    void connection::do_async_write()
    {
        util::RpcMsgPackagePtr p;
        {
            boost::mutex::scoped_lock l(list_mutex_);

            if (!list_output_.empty())
                p = list_output_.front();
        }

        if (p)
        {
            processing_ = true;

            boost::asio::async_write(socket_, p->to_buffers(),
                boost::bind(&connection::handle_write, shared_from_this(),
                boost::asio::placeholders::error, p));
        }
        else
        {
            processing_ = false;
        }

    }

    void connection::start()
    {
        remote_addr_ = socket_.remote_endpoint().address().to_string();
        connected_stat_ = connection_connected;
        connection_mng_.handle_connection(shared_from_this());

        util::RpcMsgPackagePtr recv(new util::RpcMsgPackage);
        boost::asio::async_read(socket_, boost::asio::buffer(&recv->m_header, sizeof(recv->m_header)),
            boost::bind(&connection::handle_read_head, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred, recv));

        util::RpcMsgPackagePtr p(new util::RpcMsgPackage(util::msg_type_first));
        p->setData(callback_handler_.getMyName());

        boost::asio::async_write(socket_, p->to_buffers(),
            boost::bind(&connection::handle_write, shared_from_this(),
                boost::asio::placeholders::error, p));

        deadline_.expires_from_now(boost::posix_time::seconds(5));
        deadline_.async_wait(boost::bind(&connection::check_connection_timer, shared_from_this()));
        stopped_ = false;
    }

    void connection::stop()
    {
        //if (!stopped_)
        {
            stopped_ = true;
            boost::system::error_code ec;
            socket_.close(ec);
            deadline_.cancel(ec);
        }
    }

    void connection::handle_disconnection()
    {
        connected_stat_ = connection_disconnected;
        connection_mng_.handle_connection(shared_from_this());
        callback_handler_.handle_cmd_reply_no_block(nullptr, shared_from_this(), reply_handle_service_);
        connecting_notify();
    }

    void connection::handle_read_head(const boost::system::error_code& e,
        std::size_t bytes_transferred, util::RpcMsgPackagePtr p) {
        if (!e) {
            if (package_check_number == p->m_header.iCheckByte) {
                if (!p->m_header.uDataLen) {
                    RpcMsgPostHandler(p);
                    p.reset(new util::RpcMsgPackage);
                    boost::asio::async_read(socket_, boost::asio::buffer(&p->m_header, util_header_len),
                        boost::bind(&connection::handle_read_head, shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred, p));
                }
                else {
                    // p->m_sData.resize(p->m_header.uDataLen); 
                    p->m_RecvBuff.reset(new char[p->m_header.uDataLen]); //efficient for large data
                    boost::asio::async_read(socket_, boost::asio::buffer(p->m_RecvBuff.get(), p->m_header.uDataLen),
                        boost::bind(&connection::handle_read_body, shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred, p));
                }
            }
        }
        else {
            NTB_LOG_INFO(callback_handler_.getMyName() << " lost a connection:" << e.message());
            handle_disconnection();
        }
    }

    void connection::handle_read_body(const boost::system::error_code& e,
        std::size_t bytes_transferred, util::RpcMsgPackagePtr p)
    {
        if (!e) {
            p->swapData();
            RpcMsgPostHandler(p);
            p.reset(new util::RpcMsgPackage);
            boost::asio::async_read(socket_, boost::asio::buffer(&p->m_header, util_header_len),
                boost::bind(&connection::handle_read_head, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred, p));
        }
        else{
            NTB_LOG_INFO(callback_handler_.getMyName() << " lost a connection:" << e.message());
            handle_disconnection();
        }
    }

    void connection::handle_write(const boost::system::error_code& e, util::RpcMsgPackagePtr p)
    {
        if (!e)
        {
            {
                boost::mutex::scoped_lock l(list_mutex_);
                if (!list_output_.empty())
                {
                    list_output_.pop_front();
                }
            }

            do_async_write();
        }
        else
        {
            processing_ = false;
            NTB_LOG_INFO(callback_handler_.getMyName() << " lost a connection:" << e.message());
            handle_disconnection();
        }
    }

    void connection::check_connection_timer()
    {
        if (stopped_)
            return;

        if (connection_name_get != connected_stat_) {
            NTB_LOG_INFO("don't know who it is, passive disconnect.");
            stop();
            return;
        }
        else if (connection_name_get == connected_stat_) {
#ifndef _DEBUG
            if (!processing_) {
                push_message(util::RpcMsgPackagePtr(new util::RpcMsgPackage(util::msg_type_heartbeat)));
            }

            if ((boost::posix_time::second_clock::universal_time() - heartbeat_pt_).total_seconds() > 10) {
                NTB_LOG_INFO("long time no message received, passive disconnect.peer=" << peer_name_);
                stop();
                return;
            }
#endif
            deadline_.expires_from_now(boost::posix_time::seconds(5));
            deadline_.async_wait(boost::bind(&connection::check_connection_timer, shared_from_this()));
        }
    }
}