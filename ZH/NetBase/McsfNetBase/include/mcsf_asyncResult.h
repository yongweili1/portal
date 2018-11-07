////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_asyncResult.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/01
///
/// \brief  the asynchronous command request result
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_ASYNCRESULT_H_
#define _MCSF_ASYNCRESULT_H_

#include <boost/shared_ptr.hpp>

#include "mcsf_netbase_interface.h"

namespace Mcsf {
    
     /// \class AsyncResult
     /// \brief the asynchronous command request result
    class AsyncResult : public IAsyncResult, public IDumper
    {
    public:
        AsyncResult();
        virtual ~AsyncResult();
        ICommandCallbackHandler* GetCallback() {
            return m_pReponseCallback;
        }
        void SetCallback(const ICommandCallbackHandler* pCallback) {
            m_pReponseCallback = const_cast<ICommandCallbackHandler*>(pCallback);
        }

        /// get the marshal result
        virtual std::string GetMarshalObject() const {
            return m_pSerializeObject == NULL ? std::string("") : *m_pSerializeObject;
        }

        ///set the marshal result
        void SetMarshalObject( boost::shared_ptr<std::string> ps ) {
            m_pSerializeObject = ps;
        }

        /// set complete flag 
        virtual void SetCompleted() {
            m_bComplete = true;
        }

        /// get if asynchronous command result is receive
        virtual bool IsCompleted() const {
            return m_bComplete;
        }
        /// set the sender 
        void SetSender(const std::string& sSender) {
            m_sSender = sSender;
        }
        /// get the sender 
        virtual std::string& GetSender() {
            return m_sSender;
        }

        virtual CallResult GetCallResult() const
        {
            return m_callResult;
        }

        void SetCallResult(CallResult result)  
        {
            m_callResult = result;
        }

        virtual IAsyncResult* Clone() const {
            return new AsyncResult(*this);
        }

        virtual void Destory() {
            delete this;
        }

        std::string Dump();
    private:
        ICommandCallbackHandler* m_pReponseCallback;
        boost::shared_ptr<std::string> m_pSerializeObject;
        bool m_bComplete;
        std::string m_sSender;
        CallResult m_callResult;
        

    };

}

#endif