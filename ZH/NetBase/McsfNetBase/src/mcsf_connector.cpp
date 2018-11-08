#include "stdafx.h"

#define connect_unknown_error 137

namespace Mcsf {
    Connector::Connector()
    {
        CErrorHandler::RegisterToDump(this);
    }

    Connector::~Connector()
    {
        CErrorHandler::UnRegisterToDump(this);
    }


    int Connector::Connect( TcpHandler* handler, 
        const ACE_INET_Addr& remoteAddr, 
        const ACE_Time_Value& tv, bool bRetryBindDiffIP)
    {
        NTB_TRACE_INFO_WITH_THIS("Connect enter.remoteAddr="<<remoteAddr.get_host_addr()<<" port="<<remoteAddr.get_port_number());

        ACE_SOCK_Connector connetor;
        int iRtn = connetor.connect( handler->peer(), remoteAddr, &tv );
        if( iRtn == -1  && bRetryBindDiffIP)
        {
            ACE_INT32 nErr = ACE_OS::last_error();
            if (connect_unknown_error == nErr)
            {
                std::string sClientIPs = TcpServer::GetIpInterface();
                std::vector<std::string> results;
                boost::algorithm::split(results, sClientIPs, boost::is_any_of("&"));
                if (results.size() > 1) //retry with binding different address
                {
                    BOOST_FOREACH(std::string ip, results)
                    {
                        ACE_INET_Addr bindAddr(ip.append(":0").c_str());
                        iRtn = connetor.connect( handler->peer(), remoteAddr, &tv, bindAddr );
                        if (iRtn == McsfSuccess)
                        {
                            NTB_LOG_INFO_WITH_THIS("multi-card, connect succeed with " << ip);
                            break;
                        }
                        else {
                            nErr = ACE_OS::last_error();
                            NTB_LOG_INFO_WITH_THIS("connect with bind ip '" << ip << "' failed." << " nErr="<<nErr);
                        }
                    }
                }
            }

            if (iRtn == -1)
            {
                NTB_LOG_ERROR_WITH_THIS("connect after.remoteAddr="<<remoteAddr.get_host_addr()<<" port="<<remoteAddr.get_port_number()<<" nErr="<<nErr);
                return McsfSocketConnectFailed;
            }
        }

        if( handler->open(  ) == -1 ) 
        {
            ACE_INT32 nErr = ACE_OS::last_error();
            NTB_LOG_ERROR_WITH_THIS("handler open failed.remoteAddr="<<remoteAddr.get_host_addr()<<" port="<<remoteAddr.get_port_number()<<" nErr="<<nErr);
            return McsfRegisterReactorFailed;
        }

        NTB_TRACE_INFO_WITH_THIS("connect succeed.handler="<<handler);

        return McsfSuccess;
    }


    bool Connector::IsAbleConnectTo( const std::string &sAddr )
    {
        ACE_SOCK_Connector connetor;
        ACE_SOCK_Stream stream;
        ACE_Time_Value tv(1);
        ACE_INET_Addr addr;
        addr.set(sAddr.c_str());
        int iRtn = connetor.connect(stream, addr, &tv);
        stream.close();
        return 0 == iRtn;
    }

    std::string Connector::Dump()
    {
        MCSF_TRY
            MCSF_DUMP(m_sDumpInfo, "(void)*", "this:", this );
            return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
            return ""; 
        MCSF_TRY_CATCH_END

    }

}