#pragma once

#ifndef __async_blob_data_callback_handler__
#define __async_blob_data_callback_handler__

#include "icom_proxy.h"
#include "mcsf_blobdata_interface.h"

namespace Mcsf {

    class AsyncBigDataCallBackHandler : public ICommandCallbackHandler
    {
    public:
        explicit AsyncBigDataCallBackHandler(
            boost::shared_ptr<IDataSender> pSender,
            boost::shared_ptr<ICommandCallbackHandlerEx> pCallback) 
            : m_pFileSender(pSender),
            m_pCallback(pCallback)
        {

        }
        
        virtual int HandleReply(const IAsyncResult* pResult)
        {
            boost::shared_ptr<Mcsf::AsyncResult> result(new Mcsf::AsyncResult);
            result->SetCompleted();
            result->SetCallResult(pResult->GetCallResult());
            std::string outData(pResult->GetMarshalObject());
            if (!outData.empty())
            {
                McsfSendDataResponse response;
                response.ParseFromString(outData);
                boost::shared_ptr<std::string> pObj(new std::string);
                pObj->assign(std::move(response.contents()));
                result->SetMarshalObject(pObj);
            }

            if (m_pCallback)
            {
                m_pCallback->HandleReply(result);
            }

            m_pCallback.reset();
            result.reset();
            m_pFileSender.reset();
            delete this;
            return 0;
        }

    private:
        boost::shared_ptr<IDataSender> m_pFileSender;
        boost::shared_ptr<ICommandCallbackHandlerEx> m_pCallback;
    };
}
#endif
