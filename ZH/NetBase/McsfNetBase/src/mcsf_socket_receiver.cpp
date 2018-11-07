#include "stdafx.h"

static const ACE_Time_Value kTime1(0, 200 * 1000);
static const ACE_Time_Value kTime2(0, 300 * 1000);

namespace Mcsf {


    SocketReceiver::SocketReceiver( 
        const std::string& sPeerAddr ) : m_sPeerAddr( sPeerAddr ), 
        m_pBuffer()
    {
        CErrorHandler::RegisterToDump(this);
    }

    SocketReceiver::~SocketReceiver()
    {
        CErrorHandler::UnRegisterToDump(this);
    }

    boost::shared_array<char> SocketReceiver::GetRawData( size_t& size ,bool bIsSyncCmd)
    {
        ACE_SOCK_Connector connetor;
        ACE_SOCK_Stream peer;

        std::vector<std::string> result;
        boost::algorithm::split(result, m_sPeerAddr, boost::is_any_of("&:"));
        std::string port = result.back();
        result.pop_back();
        int iRet = McsfUndefineError;
        std::vector<ACE_Time_Value> vtTimeout = boost::assign::list_of(kTime1)(kTime2);

        NTB_LOG_INFO_WITH_THIS("[blob data] begin connect to " << m_sPeerAddr);

        MCSF_TRY
            //first we use short time out to connect ,then we use long time out to connect 
            for(std::vector<ACE_Time_Value>::const_iterator itr_timeout = vtTimeout.begin();
                itr_timeout != vtTimeout.end(); itr_timeout++)
            {
                for(std::vector<std::string>::const_iterator itr = result.begin(); 
                    itr != result.end(); itr++)
                {
                    std::string ipPort = *itr + ":" + port;
                    ACE_INET_Addr remoteAddr(ipPort.c_str());
                    iRet = connetor.connect(peer, remoteAddr, &(*itr_timeout));
                    if( iRet == McsfSuccess )
                    {
                        NTB_LOG_INFO_WITH_THIS( "[blob data] connect to " << ipPort << " success." );
                        break;
                    }
                }

                if( iRet == McsfSuccess )
                {
                    break;
                }
            }

            if(iRet != McsfSuccess)
            {
                NTB_LOG_ERROR_WITH_THIS("[blob data] connect failed. iRet=" << iRet << " m_sPeerAddr=" << m_sPeerAddr << " last error=" << ACE_OS::last_error());
                size = 0;
                return boost::shared_array<char>();
            }

            int bufsiz = ACE_DEFAULT_MAX_SOCKET_BUFSIZ;
            peer.set_option(SOL_SOCKET, SO_SNDBUF, &bufsiz, sizeof bufsiz);

            size_t iLen = 0;
            if( -1 == peer.recv_n(&iLen, sizeof(iLen), &kContentTimeout) )
            {
                NTB_LOG_ERROR_WITH_THIS( "[blob data] error happen when receive blob data via socket.m_sPeerAddr="<<m_sPeerAddr << " last error=" << ACE_OS::last_error());
                peer.close();
                return boost::shared_array<char>();
            }

            m_pBuffer = boost::shared_array<char>(new char[iLen]);
            CHECK_NULL_PTR(m_pBuffer.get());
            if( -1 == peer.recv_n( m_pBuffer.get(), iLen , &kContentTimeout))
            {
                NTB_LOG_ERROR_WITH_THIS( "[blob data] error happen when receive blob data via socket.m_sPeerAddr="<<m_sPeerAddr);
                peer.close();
                return boost::shared_array<char>();
            }

            size = iLen;
            peer.close();
            return m_pBuffer;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS( "[blob data] SocketReceiver::GetRawData occurs error!m_sPeerAddr="<<m_sPeerAddr);
            peer.close();
            return boost::shared_array<char>();
        MCSF_TRY_CATCH_END
    }

    std::string SocketReceiver::Dump()
    {
        MCSF_TRY
            MCSF_DUMP(m_sDumpInfo, "(void)*", "this:", this );
            MCSF_DUMP(m_sDumpInfo, "string", "m_sPeerAddr:", m_sPeerAddr);
            MCSF_DUMP(m_sDumpInfo, "(void)*", "m_pBuffer:", &m_pBuffer);
            return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
            return ""; 
        MCSF_TRY_CATCH_END
    }
}
