#ifndef HTTP_SERVER2_IO_SERVICE_POOL_HPP
#define HTTP_SERVER2_IO_SERVICE_POOL_HPP

#include <vector>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace netbasecore{
    /// A pool of io_service objects.
    class io_service_pool
        : private boost::noncopyable
    {
    public:
        /// Construct the io_service pool.
        explicit io_service_pool(std::size_t io_size, std::size_t msg_handle_size);

        /// Run all io_service objects in the pool.
        void join_all();

        /// Stop all io_service objects in the pool.
        void stop();

        /// Get an io_service to use.
        boost::asio::io_service& get_io_service();

        boost::asio::io_service& get_msg_handle_service();

        boost::asio::io_service& get_reply_handle_service();

    private:
        typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
        typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;

        std::vector<boost::shared_ptr<boost::thread> > threads_;

        /// The pool of io_services.
        std::vector<io_service_ptr> io_services_;

        /// The work that keeps the io_services running.
        std::vector<work_ptr> work_;

        /// The next io_service to use for a connection.
        std::size_t next_io_service_;

        std::size_t next_msg_handle_service_;

        std::size_t io_service_size_;

        boost::mutex mutx_;
    };

}

#endif // HTTP_SERVER2_IO_SERVICE_POOL_HPP