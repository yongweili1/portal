////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_connector.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/04 
///
/// \brief  A Connector base on ACE Connector pattern 
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_CONNECTOR_H_
#define _MCSF_CONNECTOR_H_

#include <ace/SOCK_Connector.h> 

namespace Mcsf {

    class TcpHandler;

    class Connector: public IDumper
    {
    public:
        Connector();
        ~Connector();
        /// connect \a remoteAddr to the peer of \a handler
        int Connect( TcpHandler* handler, 
            const ACE_INET_Addr& remoteAddr, 
            const ACE_Time_Value& tv, bool bRetryBindDiffIP = false); 
        static bool IsAbleConnectTo(const std::string &sAddr);
        std::string Dump();
    };

}

#endif