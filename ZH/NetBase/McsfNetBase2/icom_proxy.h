#pragma once

#ifndef ICOM_PROXY_H
#define ICOM_PROXY_H

#include "../include/McsfNetBase/mcsf_netbase_interface.h"
#include "../include/McsfNetBase/mcsf_error_code.h"

namespace Mcsf{
    class SyncResult : public ISyncResult
    {
        friend class CommunicationProxy;
    public:
        SyncResult() : m_sResult("")
        {
            // default failed
            m_iResult = -1;
        }

        /// get the response marshal result
        virtual std::string GetSerializedObject()
        {
            return m_sResult;
        }

        /// set the response result
        virtual void SetSerializedObject(const std::string& sResult)
        {
            m_sResult = sResult;
        }

        /// get the call result
        virtual int GetCallResult() {
            return m_iResult;
        }

        /// set call result
        void SetCallResult(int iret)
        {
            m_iResult = iret;
        }

    private:
        int m_iResult;              /// the request call result
        std::string m_sResult;      /// marshaled response result
    };

    class AsyncResult : public IAsyncResult{
    public:
        AsyncResult() {}
        virtual ~AsyncResult() {}
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
        void SetMarshalObject(boost::shared_ptr<std::string> ps) {
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

    private:
        ICommandCallbackHandler* m_pReponseCallback;
        boost::shared_ptr<std::string> m_pSerializeObject;
        bool m_bComplete;
        std::string m_sSender;
        CallResult m_callResult;
    };
}

typedef boost::shared_ptr<Mcsf::IEventHandler> McsfIEventHandlerPtr;
typedef boost::shared_ptr<Mcsf::ICommandHandlerEx> McsfICommandHandlerExPtr;
typedef boost::shared_ptr<Mcsf::ICommandCallbackHandlerEx> McsfICommandCallbackHandlerExPtr;

#endif
