////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_socket_sender.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/14
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_SOCKET_SENDER_H_
#define MCSF_SOCKET_SENDER_H_

#include "boost/thread.hpp"
#include <boost/asio.hpp>

#include "mcsf_blobdata_interface.h"

namespace Mcsf { 

    class SocketSender : public IDataSender
    {
    public:
        SocketSender();
        virtual ~SocketSender();

        bool SendRawData( boost::shared_array<char> pBuffer, size_t iLen ); 

        virtual void SetTimeOut( int iMillSecond )
        {
            m_iTimeout = iMillSecond;
        }

        void SetName(const std::string& sName) 
        {
            m_sIpAddr = sName;
        }

        std::string GetName() const 
        {
            return m_sIpAddr;
        }

        virtual int GetSendType() const 
        {
            return send_data_by_socket;
        }

        void handle_accept(const boost::system::error_code &ec);
        void handle_write_finished(const boost::system::error_code &ec);
        void handle_timeout(const boost::system::error_code &ec);
        void DestoryRawData( );
        void CloseDataFile();

    private:

        std::string m_sIpAddr;
        boost::shared_array<char> m_pBuffer;
        size_t m_iLen;
        int m_iTimeout;

        boost::thread thread_send_;
        boost::asio::io_service io;

        boost::asio::ip::tcp::socket socket_;
        boost::asio::ip::tcp::acceptor m_acceptor;
        boost::asio::deadline_timer dead_line_;
    };
}

#endif // MCSF_SOCKET_SENDER_H_
