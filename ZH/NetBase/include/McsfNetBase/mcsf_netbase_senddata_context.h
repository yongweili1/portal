////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_netbase_senddata_context.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/07
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_NETBASE_SENDDATA_CONTEXT_H_
#define MCSF_NETBASE_SENDDATA_CONTEXT_H_

#include <string>
#include "boost/shared_ptr.hpp"
#include "boost/shared_array.hpp"

namespace Mcsf{
    class ICommandCallbackHandlerEx;
    struct SendDataContext
    {
        std::string sReceiver;   /// the container name of receiver
        std::string sSender;     /// reserved for forward compatible 
        int iCommandId;          ///reserved not used current
        std::string sSerailizedObject; ///info you may be let service side known
        unsigned int iLen;       /// the length of raw data
        void* pRawData;          ///the raw data start point 
        /// if 0, synchronous command will block until response
        /// return, other wait for specify time, unit is ms. 
        /// if timer out command will return non-zero,
        /// and result will drop by net base. only for sync command 
        int iWaitTime; 

    public:
        SendDataContext() : sReceiver(""), sSender(""), iCommandId(0), sSerailizedObject("")
        {
            iLen = 0;
            pRawData = NULL;
            iWaitTime = 0;
        }
         
        /// allocate native memory, so manage code can copy raw data to native stack
        /// don't use in pure C++ code.
        /// 
        /// \param   int iLen         the length of memory
        /// \return  void 
        void AllocMem( int iLen )
        {
            if( pRawData != NULL )
            {
                delete[] (char*)pRawData;
            }

            pRawData = new char[iLen];
        }

        ///don't use in pure C++ code
        void DestoryMem(  )
        {
            if( pRawData != NULL )
            {
                delete[] (char*)pRawData;
                pRawData = NULL;
            }
        }

    };

    struct SendDataContextEx 
    {
        std::string sReceiver;   ///required, the container name of receiver
        std::string sSender;     ///optional, reserved for forward compatible 
        int iCommandId;          ///optional,reserved not used current
        std::string sSerailizedObject; ///optional,info you may be let service side known
        unsigned int iLen;       ///required if @pRawData is not null, the length of raw data
        boost::shared_array<char> pRawData;          ///optional,the raw data start point 
        boost::shared_ptr<ICommandCallbackHandlerEx> pCallback;//optional
        unsigned int iWaitTime; //optional, 0 means wait forever
        std::string sResponseObject;  ///response serialized object 

    public:
        SendDataContextEx() : sReceiver(""), sSender(""), 
            iCommandId(0), sSerailizedObject(""),
            iLen(0), pRawData(), pCallback(), 
            iWaitTime(0), sResponseObject("")
        {
            
        }
    };
}

#endif // MCSF_NETBASE_SENDDATA_CONTEXT_H_