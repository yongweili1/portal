////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_socket_receiver.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/14
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_SOCKET_RECEIVER_H_
#define MCSF_SOCKET_RECEIVER_H_

#include <string>
#include "boost/shared_array.hpp"
#include "mcsf_blobdata_interface.h"

namespace Mcsf {

    class SocketReceiver : public IDataReceiver, public IDumper
    {
    public:
        SocketReceiver( const std::string& sPeerAddr );
        ~SocketReceiver(  );

        virtual boost::shared_array<char> GetRawData( size_t& size , bool bIsSyncCmd = false) ;
        std::string Dump();
    private:
        std::string m_sPeerAddr;
        boost::shared_array<char> m_pBuffer;
    };
}

#endif // MCSF_SOCKET_RECEIVER_H_
