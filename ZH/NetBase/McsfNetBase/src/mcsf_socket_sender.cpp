#include "stdafx.h"

namespace Mcsf {

    SocketSender::SocketSender() : m_sIpAddr("")
    {
        CErrorHandler::RegisterToDump(this);
        m_iLen = 0;
        m_iTimeout = 0;
    }

    SocketSender::~SocketSender()
    {
        CErrorHandler::UnRegisterToDump(this);
        m_acceptor.close();
        this->wait();
    }

    bool SocketSender::SendRawData( boost::shared_array<char> pBuffer, size_t iLen )
    {
        static const std::string sLocalHost = "127.0.0.1";
        if( pBuffer == NULL || iLen > MAX_SHARE_MEMORY_CAPCITY )
        {
            return false;
        }
        m_pBuffer = pBuffer;
        m_iLen = iLen;

        ACE_INET_Addr portToListen;
        portToListen.set("0:0");

        if( m_acceptor.open( portToListen , 1) == -1 )
        {
            NTB_LOG_ERROR_WITH_THIS( "[blob data]accept open failed.");
            return false;
        }

        m_acceptor.get_local_addr( portToListen );
        m_sIpAddr = TcpServer::GetIpInterface() + ":"
            + boost::lexical_cast<std::string>(portToListen.get_port_number());

        return (0 == this->activate()) ? true : false;
    }

    int SocketSender::svc( void ) 
    {
        MCSF_TRY

            ACE_SOCK_Stream peer;
            ACE_INET_Addr peer_addr;
            ACE_Time_Value *pTimeout = NULL;
            ACE_Time_Value tv_TimeOut( 0, m_iTimeout * 1000 );
            if( m_iTimeout > 0 )
            {
                pTimeout = &tv_TimeOut;
            }

            if( m_acceptor.accept( peer, &peer_addr,pTimeout ) == -1 )
            {
                if( ACE_OS::last_error() == EINTR )
                {
                    NTB_LOG_ERROR_WITH_THIS( "[blob data] Interrupted while waiting for connection." );
                }
                else if( ACE_OS::last_error() == ETIMEDOUT )
                {
                    NTB_LOG_ERROR_WITH_THIS( "[blob data] Timeout while waiting for connection" );
                }
            }
            else
            {
                ACE_TCHAR peer_name[MAXHOSTNAMELEN];
                peer_addr.addr_to_string( peer_name, MAXHOSTNAMELEN );
                NTB_LOG_INFO_WITH_THIS( "[blob data] Connection from " << peer_name );

                int bufsiz = ACE_DEFAULT_MAX_SOCKET_BUFSIZ;
                peer.set_option(SOL_SOCKET, SO_SNDBUF, &bufsiz, sizeof bufsiz);

                if( -1 == peer.send_n( &m_iLen, sizeof(m_iLen), &kHeaderTimeout ))
                {
                    NTB_LOG_ERROR_WITH_THIS("[blob data] error happen while send data len.m_sIpAddr=" << m_sIpAddr << " m_iLen" << m_iLen << " last error:" << ACE_OS::last_error());
                    peer.close();
                    return -1;
                }

                if( peer.send_n( m_pBuffer.get(), m_iLen, &kContentTimeout ) < 0 )
                {
                    NTB_LOG_ERROR_WITH_THIS( "[blob data]error happen while send data.m_sIpAddr="<<m_sIpAddr<<" m_iLen"<<m_iLen << " last error:" << ACE_OS::last_error());
                }
            }

            peer.close();
            return 0;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS( "SocketSender::svc occurs error!m_sIpAddr="<<m_sIpAddr<<" m_iLen"<<m_iLen);
            return -1;
        MCSF_TRY_CATCH_END
    }

    void SocketSender::DestoryRawData( )
    {
        return;
    }

    void SocketSender::CloseDataFile( )
    {
        return;
    }    

    std::string SocketSender::Dump()
    {
        MCSF_TRY
            MCSF_DUMP(m_sDumpInfo, "(void)*", "this:", this );
            MCSF_DUMP(m_sDumpInfo, "string", "m_sIpAddr:", m_sIpAddr);
            MCSF_DUMP(m_sDumpInfo, "(void)*", "&m_pBuffer:", &m_pBuffer);
            MCSF_DUMP(m_sDumpInfo, "size_t", "m_iLen:", m_iLen);
            MCSF_DUMP(m_sDumpInfo, "int", "m_iTimeout:", m_iTimeout);
            return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
            return "";
        MCSF_TRY_CATCH_END
    }
}

