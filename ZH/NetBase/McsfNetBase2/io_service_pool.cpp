#include <stdexcept>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "io_service_pool.hpp"
#include "mcsf_refined_log.h"


namespace netbasecore {
    io_service_pool::io_service_pool(std::size_t io_size, std::size_t msg_handle_size)
        : io_service_size_(io_size), next_io_service_(0), next_msg_handle_service_(io_size)
    {
        if (io_size == 0 || msg_handle_size == 0)
            throw std::runtime_error("io_service_pool size is 0");

        boost::mutex::scoped_lock lc(mutx_);
        // Give all the io_services work to do so that their run() functions will not
        // exit until they are explicitly stopped.
        for (std::size_t i = 0; i < io_size + msg_handle_size + 1; ++i)
        {
            io_service_ptr io_service(new boost::asio::io_service);
            work_ptr work(new boost::asio::io_service::work(*io_service));
            io_services_.push_back(io_service);
            work_.push_back(work);
        }

        NTB_LOG_INFO("io_service_pool::run begin");
        // Create a pool of threads to run all of the io_services.
        for (std::size_t i = 0; i < io_services_.size(); ++i)
        {
            boost::shared_ptr<boost::thread> thread(new boost::thread(
                boost::bind(&boost::asio::io_service::run, io_services_[i])));
            NTB_LOG_INFO("boost::asio::io_service::run id:" << thread->get_id());
            threads_.push_back(thread);
        }

        NTB_LOG_INFO("io_service_pool end");
    }

    void io_service_pool::join_all()
    {
        // Wait for all threads in the pool to exit.
        for (std::size_t i = 0; i < threads_.size(); ++i)
            threads_[i]->join();
    }

    void io_service_pool::stop()
    {
        boost::mutex::scoped_lock lc(mutx_);
        // Explicitly stop all io_services.
        for (std::size_t i = 0; i < io_services_.size(); ++i)
            io_services_[i]->stop();
    }

    boost::asio::io_service& io_service_pool::get_io_service()
    {
        boost::mutex::scoped_lock lc(mutx_);

        //NTB_LOG_INFO("get_io_service enter.");
        // Use a round-robin scheme to choose the next io_service to use.
        boost::asio::io_service& io_service = *io_services_[next_io_service_];
        ++next_io_service_;
        if (next_io_service_ == io_service_size_)
            next_io_service_ = 0;
        return io_service;
    }

    boost::asio::io_service& io_service_pool::get_msg_handle_service()
    {
        boost::mutex::scoped_lock lc(mutx_);

        // Use a round-robin scheme to choose the next io_service to use.
        boost::asio::io_service& io_service = *io_services_[next_msg_handle_service_];
        //NTB_LOG_INFO("get_msg_handle_service enter.");
        ++next_msg_handle_service_;
        if (next_msg_handle_service_ == io_services_.size() - 1)
            next_msg_handle_service_ = io_service_size_;
        return io_service;
    }

    boost::asio::io_service& io_service_pool::get_reply_handle_service()
    {
        boost::mutex::scoped_lock lc(mutx_);
        //NTB_LOG_INFO("get_reply_handle_service enter.");
        return *io_services_[io_services_.size() - 1];
    }

}