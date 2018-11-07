#pragma once

#ifndef __send_data_cmd_handler_h__
#define __send_data_cmd_handler_h__
#endif

#include <boost/scoped_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "icom_proxy.h"
#include "SystemCmdMessage.pb.h"
#include "mcsf_blobdata_interface.h"
#include "mcsf_share_memory_receiver.h"
#include "mcsf_socket_receiver.h"

namespace Mcsf {
    class SendDataCmdHandler : public ICommandHandlerEx
    {
    public:
        explicit SendDataCmdHandler(IDataHandler* pHandler)
            : m_pDataHandler(pHandler), m_work(m_io) {
            m_thread = (boost::thread)(boost::bind(&boost::asio::io_service::run, boost::ref(m_io)));
        }

        explicit SendDataCmdHandler(boost::shared_ptr<IDataHandlerEx> pHandler)
            : m_pDataHandlerEx(pHandler), m_pDataHandler(nullptr), m_work(m_io) {
            m_thread = (boost::thread)(boost::bind(&boost::asio::io_service::run, boost::ref(m_io)));
        }

        ~SendDataCmdHandler() {
            stop();
        }

        void HandleCommand(boost::shared_ptr<ICommandContextEx> pContext)
        {
            m_io.post(boost::bind(&SendDataCmdHandler::ReceiveData, this, pContext));
        }

        void stop() {
            if (!m_io.stopped()) 
                m_io.stop();

            if (m_thread.joinable())
                m_thread.join();
        }

        virtual void ReceiveData(boost::shared_ptr<ICommandContextEx> pContext)
        {
            McsfSendDataRequest request;
            request.ParseFromString(pContext->GetSerializeObject());

            boost::scoped_ptr<IDataReceiver> pReceiver;
            if ((int)request.type() == IDataSender::send_data_by_sharememory){
                pReceiver.reset(new SMFileReceiver(request.name()));
            }
            else{
                pReceiver.reset(new SocketReceiver(request.name()));
            }

            size_t iLen = 0;

            bool bIsSyncCmd = request.is_sync();
            boost::shared_array<char> pData = pReceiver->GetRawData(iLen, bIsSyncCmd);

            McsfSendDataResponse response;
            response.set_is_success(false);

            if (pData != NULL)
            {
                if (m_pDataHandlerEx != NULL)
                {
                    boost::shared_ptr<SendDataContextEx> pDataContext(new SendDataContextEx);
                    pDataContext->sSender = pContext->GetSender();
                    pDataContext->sSerailizedObject = request.content();
                    pDataContext->pRawData = pData;
                    pDataContext->iLen = iLen;
                    m_pDataHandlerEx->HandleDataTrans(pDataContext);
                    response.set_contents(pDataContext->sResponseObject);
                    response.set_is_success(true);
                }
                else if (m_pDataHandler != NULL)
                {
                    m_pDataHandler->HandleDataTrans(pData.get(), iLen);
                    response.set_is_success(true);
                }
                else
                {
                    NTB_LOG_ERROR("[blob data]data handler is null, or not binding data handler!");
                    response.set_is_success(false);
                }
            }
            else
            {
                NTB_LOG_ERROR("[blob data]GetRawData is null!");
                response.set_is_success(false);
            }

            pContext->Reply(response.SerializeAsString());
        }


    private:
        IDataHandler* m_pDataHandler;
        boost::shared_ptr<IDataHandlerEx> m_pDataHandlerEx;
        boost::asio::io_service m_io;
        boost::asio::io_service::work m_work;
        boost::thread m_thread;
    };
}