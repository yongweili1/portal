//==========================================================================
/**
 *  Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
 *  All rights reserved
 *
 *  @file   mcsf_tcp_client.h
 *  @author wenbin.tu.ext <wenbin.tu.ext@united-imaging.com>
 * 
 *  @date   2011/09/05
 *
 *  @brief  represent tcp client 
 */
//==========================================================================

#ifndef _MCSF_COMMUNICATION_PROXY_FACTORY_H
#define _MCSF_COMMUNICATION_PROXY_FACTORY_H

//c++ header
#include <string>

//inner-header 
#include "mcsf_netbase_export.h"

namespace Mcsf
{
	class ICommunicationProxy;
}

namespace Mcsf
{
    class CommunicationProxyFactory 
    {
        static McsfNetBase_Export ICommunicationProxy* Init( const std::string& sConfigFile );
    };
         
}

#endif