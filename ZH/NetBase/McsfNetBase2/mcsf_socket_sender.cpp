#include "mcsf_socket_sender.h"

#include "boost/lexical_cast.hpp"
#include "mcsf_refined_log.h"

namespace Mcsf {

    SocketSender::SocketSender()
        : m_sIpAddr("") , m_acceptor(io), socket_(io), dead_line_(io)
    {
        m_iLen = 0;
        m_iTimeout = 0;
    }

    SocketSender::~SocketSender()
    {
        m_acceptor.close();
        io.stop();

        if (thread_send_.joinable())
            thread_send_.join();
    }

    bool SocketSender::SendRawData( boost::shared_array<char> pBuffer, size_t iLen )
    {
        static const std::string sLocalHost = "127.0.0.1";
        if( pBuffer == NULL || iLen > MAX_SHARE_MEMORY_CAPCITY )
        {
            NTB_LOG_ERROR("[blob data]invalid parameters! pBuffer=" << pBuffer.get() << " ilen=" << iLen);
            return false;
        }
        m_pBuffer = pBuffer;
        m_iLen = iLen;

        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 0);
        m_acceptor.open(endpoint.protocol());
        m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        m_acceptor.bind(endpoint);
        m_acceptor.listen();

        if (m_sIpAddr.empty())
        {
            NTB_LOG_WARN("[blob data]not found m_sIpAddr. using default!");
            m_sIpAddr = boost::asio::ip::address_v4::loopback().to_string();
        }

        m_sIpAddr += ":" + boost::lexical_cast<std::string>(m_acceptor.local_endpoint().port());

        //TODO: timeout !!!
        if (m_iTimeout) {
            dead_line_.expires_from_now(boost::posix_time::millisec(m_iTimeout));
            dead_line_.async_wait(boost::bind(&SocketSender::handle_timeout, this, _1));
        }
        m_acceptor.async_accept(socket_, boost::bind(&SocketSender::handle_accept, this, _1));
        thread_send_ = (boost::thread)boost::bind(&boost::asio::io_service::run, boost::ref(io));

        return true;

    }

    void SocketSender::handle_accept(const boost::system::error_code &ec)
    {
        if (ec)
        {
            NTB_LOG_ERROR("[blob data] handle_accept error:" << ec.message());
            return;
        }
        boost::asio::socket_base::send_buffer_size sz(65536);
        socket_.set_option(sz);
        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(&m_iLen, sizeof(m_iLen)));
        buffers.push_back(boost::asio::buffer(m_pBuffer.get(), m_iLen));
        boost::asio::async_write(socket_, buffers, boost::bind(&SocketSender::handle_write_finished, this, _1));
    }

    void SocketSender::handle_write_finished(const boost::system::error_code &ec)
    {
        if (ec)
        {
            NTB_LOG_ERROR("[blob data] handle_write_finished error:" << ec.message());
        }

        boost::system::error_code egonr;
        socket_.close(egonr);
    }

    void SocketSender::handle_timeout(const boost::system::error_code &ec)
    {
        if (boost::asio::error::operation_aborted != ec)
        {
            NTB_LOG_ERROR("[blob data] handle_timeout enter.");
            boost::system::error_code egonr;
            socket_.close(egonr);
        }
    }

    void SocketSender::DestoryRawData()
    {
    }

    void SocketSender::CloseDataFile( )
    {
    }    
}

