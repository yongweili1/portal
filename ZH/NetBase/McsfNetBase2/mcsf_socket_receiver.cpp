#include "mcsf_socket_receiver.h"

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

#include "rpc.h"
#include "mcsf_refined_log.h"

namespace Mcsf {


    SocketReceiver::SocketReceiver( 
        const std::string& sPeerAddr ) : m_sPeerAddr( sPeerAddr ), 
        m_pBuffer()
    {
    }

    SocketReceiver::~SocketReceiver()
    {
    }

    boost::shared_array<char> SocketReceiver::GetRawData( size_t& size ,bool bIsSyncCmd)
    {
        boost::asio::io_service io;
        boost::asio::ip::tcp::socket socket_(io);
        boost::system::error_code ec;

        try
        {
            netbasecore::util::host_port_t hp(m_sPeerAddr);
            if (!hp.parsed_ok)
            {
                NTB_LOG_ERROR("[blob data]parse peer address failed!m_sPeerAddr=" << m_sPeerAddr);
                return false;
            }

            auto addr = boost::asio::ip::address::from_string(hp.host, ec);
            if (ec)
            {
                NTB_LOG_ERROR("[blob data]IP to address failed!hp=" << m_sPeerAddr);
                return nullptr;
            }
            boost::asio::ip::tcp::endpoint edp(addr, atoi(hp.port.c_str()));
            socket_.connect(edp, ec);
            if (ec)
            {
                NTB_LOG_ERROR("[blob data]connect peer failed! ec=" << ec.message() << " m_sPeerAddr=" << m_sPeerAddr);
                return nullptr;
            }

            boost::asio::socket_base::receive_buffer_size sz(65536);
            socket_.set_option(sz);

            size_t data_iLen = 0;
            size_t len = socket_.receive(boost::asio::buffer(&data_iLen, sizeof(data_iLen)), 0, ec);
            if (len != sizeof(data_iLen) || ec)
            {
                NTB_LOG_ERROR("[blob data]recv header len failed!len=" << len << " data_iLen=" << data_iLen << ec.message());
                socket_.close(ec);
                return nullptr;
            }

            m_pBuffer = boost::shared_array<char>(new char[data_iLen]);
            size_t len_recv = 0;
            while (data_iLen)
            {
                len = socket_.receive(boost::asio::buffer(m_pBuffer.get() + len_recv, data_iLen), 0, ec);
                if (ec) {
                    NTB_LOG_ERROR("[blob data]recv data failed! len_recv=" << len_recv << " data_len=" << data_iLen << ec.message());
                    socket_.close(ec);
                    return nullptr;
                }
                data_iLen -= len;
                len_recv += len;
            }

            size = len_recv;
            return m_pBuffer;
        }
        catch (...)
        {
            NTB_LOG_ERROR("[blob data]exception happend!");
            socket_.close(ec);
            return nullptr;
        }
    }
}
