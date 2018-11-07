////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_netbase_ibigdata_handler.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/07
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_NETBASE_IBIGDATA_HANDLER_H_
#define MCSF_NETBASE_IBIGDATA_HANDLER_H_

#include "boost/shared_ptr.hpp"
#include "mcsf_netbase_senddata_context.h"

namespace Mcsf {
    
    /// \class IBigDataHandler
    /// \brief the base class of send data call. it is used by big data or raw data in
    /// most case
    class IDataHandler
    {
    public:
        virtual int HandleDataTrans(void* pRawData, size_t iLen) = 0;
        virtual ~IDataHandler() {}
    };

    class IDataHandlerEx 
    {
    public:
        virtual ~IDataHandlerEx() {}
        virtual void HandleDataTrans(boost::shared_ptr<SendDataContextEx> pDataContext) = 0;
    };

}

#endif // MCSF_NETBASE_IBIGDATA_HANDLER_H_
