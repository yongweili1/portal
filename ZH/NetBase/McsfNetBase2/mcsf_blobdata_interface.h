////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_blobdata_interface.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/14
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_BLOBDATA_INTERFACE_H_
#define MCSF_BLOBDATA_INTERFACE_H_

#include <string>
#include "boost/shared_array.hpp"

namespace Mcsf {

    const int MAX_SHARE_MEMORY_CAPCITY = 500 * 1024 * 1024;

    class IDataSender 
    {
    public:
        enum {
            send_data_by_sharememory = 1,
            send_data_by_socket = 2,
        };

        virtual ~IDataSender() {}

        virtual bool SendRawData( boost::shared_array<char> pBuffer, size_t iLen ) = 0;
        virtual std::string GetName() const = 0;
        virtual void SetName(const std::string& sName ) = 0;
        virtual void SetTimeOut( int iMillSecond ) = 0;
        virtual int GetSendType() const = 0;
        virtual void DestoryRawData() = 0;
        virtual void CloseDataFile() = 0;
    };

    class IDataReceiver
    {
    public:
        virtual ~IDataReceiver() {} ;

        virtual boost::shared_array<char> GetRawData( size_t& size , bool bIsSyncCmd) = 0;
    };
}

#endif // MCSF_BLOBDATA_SENDER_INTERFACE_H_
