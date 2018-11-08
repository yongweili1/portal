#include "com_proxy_impl.hpp"

namespace Mcsf {

    CommunicationProxy::CommunicationProxy(int iReactorPriority /* = 0 */)
        : m_pImpl(NULL)
        , m_iChannelId(0)
    {
        m_iPriority = iReactorPriority;
    }

    CommunicationProxy::~CommunicationProxy()
    {
        if (m_pImpl) {
            delete m_pImpl; m_pImpl = nullptr;
        }
    }

    int CommunicationProxy::AsyncSendCommand(const CommandContext* pCommandContext)
    {
        return m_pImpl ? m_pImpl->AsyncSendCommand(pCommandContext) : McsfNotCallStartListener;
    }

    int CommunicationProxy::SyncSendCommand(const CommandContext* pCommandContext, std::string& sSerializeObject)
    {
        return m_pImpl ? m_pImpl->SyncSendCommand(pCommandContext, sSerializeObject) : McsfNotCallStartListener;
    }

    Mcsf::ISyncResult* CommunicationProxy::SyncSendCommand(const CommandContext* pCommandContext)
    {
        return m_pImpl->SyncSendCommand(pCommandContext);
    }

    int CommunicationProxy::SyncSendData(const SendDataContext* pSendDataContext)
    {
        if (nullptr == pSendDataContext) {
            NTB_LOG_ERROR("pSendDataContext is null!");
            return McsfUndefineError;
        }

        SendDataContextEx dataContexEx;
        dataContexEx.sReceiver = pSendDataContext->sReceiver;
        dataContexEx.pRawData.reset(new char[pSendDataContext->iLen]);
        memcpy(dataContexEx.pRawData.get(), pSendDataContext->pRawData, pSendDataContext->iLen);
        dataContexEx.iLen = pSendDataContext->iLen;
        dataContexEx.iCommandId = pSendDataContext->iCommandId;
        dataContexEx.iWaitTime = pSendDataContext->iWaitTime;
        dataContexEx.sSerailizedObject = pSendDataContext->sSerailizedObject;

        std::string noMeans;
        return this->SyncSendDataEx(dataContexEx, noMeans);
    }

    int CommunicationProxy::SyncSendDataEx(const SendDataContextEx& pSendContext, std::string& sSerializeObject)
    {
        return m_pImpl ? m_pImpl->SyncSendDataEx(pSendContext, sSerializeObject) : McsfNotCallStartListener;
    }

    Mcsf::ISyncResult* CommunicationProxy::SyncSendDataEx(const SendDataContextEx& pSendContext)
    {
        return m_pImpl->SyncSendDataEx(pSendContext);
    }

    int CommunicationProxy::AsyncSendData(const SendDataContext* pSendDataContext)
    {
        if (nullptr == pSendDataContext) {
            NTB_LOG_ERROR("pSendDataContext is null!");
            return McsfUndefineError;
        }

        SendDataContextEx dataContexEx;
        dataContexEx.sReceiver = pSendDataContext->sReceiver;
        dataContexEx.pRawData.reset(new char[pSendDataContext->iLen]);
        memcpy(dataContexEx.pRawData.get(), pSendDataContext->pRawData, pSendDataContext->iLen);
        dataContexEx.iLen = pSendDataContext->iLen;
        dataContexEx.iCommandId = pSendDataContext->iCommandId;
        dataContexEx.iWaitTime = pSendDataContext->iWaitTime;
        dataContexEx.sSerailizedObject = pSendDataContext->sSerailizedObject;

        return this->AsyncSendDataEx(dataContexEx);
    }

    int CommunicationProxy::AsyncSendDataEx(const SendDataContextEx& pSendContext)
    {
        return m_pImpl ? m_pImpl->AsyncSendDataEx(pSendContext) : McsfNotCallStartListener;
    }

    int CommunicationProxy::RegisterCommandHandler(int iCommandId, ICommandHandler* pCommandHandler)
    {
        return m_pImpl ? m_pImpl->RegisterCommandHandler(iCommandId, pCommandHandler) : McsfNotCallStartListener;
    }

    int CommunicationProxy::RegisterCommandHandlerEx(int iCommandId, boost::shared_ptr<ICommandHandlerEx> pCommandHandler)
    {
        return m_pImpl ? m_pImpl->RegisterCommandHandlerEx(iCommandId, pCommandHandler) : McsfNotCallStartListener;
    }

    void CommunicationProxy::UnRegisterCommandHandler(int iCommandId)
    {
        if (m_pImpl) m_pImpl->UnRegisterCommandHandler(iCommandId);
    }

    void CommunicationProxy::UnRegisterAllCommandHandlers(void)
    {
        if (m_pImpl) m_pImpl->UnRegisterAllCommandHandlers();
    }

    class DataHandler2ExAdaptor : public IDataHandlerEx
    {
    public:
        DataHandler2ExAdaptor(IDataHandler *pDataHandler) : m_pDataHandler(pDataHandler) {}
        virtual void HandleDataTrans(boost::shared_ptr<SendDataContextEx> pDataContext) {
            if (m_pDataHandler) {
                m_pDataHandler->HandleDataTrans(pDataContext->pRawData.get(), pDataContext->iLen);
            }
        }
    private:
        IDataHandler *m_pDataHandler;
    };

    int CommunicationProxy::RegisterDataHandler(IDataHandler* pSendDataHandler)
    {
        boost::shared_ptr<IDataHandlerEx> pSendDataHandlerEx(new DataHandler2ExAdaptor(pSendDataHandler));
        return this->RegisterDataHandlerEx(pSendDataHandlerEx);
    }

    int CommunicationProxy::RegisterDataHandlerEx(boost::shared_ptr<IDataHandlerEx> pSendDataHandler)
    {
        return m_pImpl ? m_pImpl->RegisterDataHandlerEx(pSendDataHandler) : McsfNotCallStartListener;
    }

    void CommunicationProxy::UnRegisterDataHandler(void)
    {
        if (m_pImpl) m_pImpl->UnRegisterDataHandler();
    }

    int CommunicationProxy::RegisterEventHandler(int iChannelId, int iEventId, IEventHandler* pEventHandler)
    {
        return m_pImpl ? m_pImpl->RegisterEventHandler(iChannelId, iEventId, pEventHandler) : McsfNotCallStartListener;
    }

    void CommunicationProxy::UnRegisterEventHandler(int iChannelId, int iEventId)
    {
        if (m_pImpl) m_pImpl->UnRegisterEventHandler(iChannelId, iEventId);
    }

    void CommunicationProxy::UnRegisterAllEventHandlers()
    {
        if (m_pImpl) m_pImpl->UnRegisterAllEventHandlers();
    }

    void CommunicationProxy::UnRegisterAllHandlers()
    {
        if (m_pImpl) m_pImpl->UnRegisterAllHandlers();
    }

    int CommunicationProxy::SendEvent(const std::string& sSender, int iEventId, const std::string& sEvent)
    {
        return m_pImpl ? m_pImpl->SendEvent(sSender, m_iChannelId, iEventId, sEvent) : McsfNotCallStartListener;
    }
// 
//     int CommunicationProxy::SendEvent(const std::string& sSender, int iEventId, std::string& sEvent)
//     {
//         return m_pImpl ? m_pImpl->SendEvent(sSender, m_iChannelId, iEventId, sEvent) : McsfNotCallStartListener;
//     }

    int CommunicationProxy::BroadcastEvent(const std::string& sSender, int iEventId, const std::string& sEvent)
    {
        return m_pImpl ? m_pImpl->BroadcastEvent(sSender, m_iChannelId, iEventId, sEvent) : McsfNotCallStartListener;
    }

    int CommunicationProxy::SubscribeEvent(const ChannelIdMap& idMap)
    {
        return 0;
    }

    int CommunicationProxy::SubscribeEvent(int iChannelId, int iEventId)
    {
        return 0;
    }

    int CommunicationProxy::StartListener(const std::string& sIpAddr)
    {
        CommunicationProxy::Implement::ImplEntry entry;
        entry.ioReactorPriority = m_iPriority;
        entry.dispatcherAddr = m_sDispatcherAddr.empty() ? "127.0.0.1:10000" : m_sDispatcherAddr;
        entry.ioThrNum = 1;
        entry.handleThreNum = 1;
        entry.name = m_sName;
        if (m_sListenPort.empty())
        {
            netbasecore::util::host_port_t hp(sIpAddr);
            m_sListenPort = hp.port;
        }
        entry.listenPort = m_sListenPort;
        try
        {
            m_pImpl = new Implement(entry);
            return McsfSuccess;
        }
        catch (std::exception &e)
        {
            NTB_LOG_INFO("startlistner failed!" << e.what());
            return McsfUndefineError;
        }
    }

    int CommunicationProxy::CheckCastToSystemDispatcher(const std::string& sDispatchIpAddr)
    {
        m_sDispatcherAddr = sDispatchIpAddr;
        if (m_pImpl)
        {
            return m_pImpl->CheckCastName(dispatcher_name, 0);
        }
        return McsfSuccess;
    }

    int CommunicationProxy::CheckCastName(const std::string& sName, int iWaitTime)
    {
        return m_pImpl ? m_pImpl->CheckCastName(sName, iWaitTime) : McsfNotCallStartListener;
    }

    void CommunicationProxy::SetName(const std::string& sName)
    {
        m_sName = sName;
    }

    std::string CommunicationProxy::GetName() const
    {
        return m_pImpl ? m_pImpl->GetName() : m_sName;
    }

    void CommunicationProxy::SetSenderChannelId(int iChannelId)
    {
        m_iChannelId = iChannelId;
    }

    int CommunicationProxy::GetSenderChannelId(void) const
    {
        return m_iChannelId;
    }

    int CommunicationProxy::DispatchEvent(const std::string& sReceiver, const std::string& sSender, int iChannelId, int iEventId, const std::string& sEvent)
    {
        return McsfNotImplemnentForDeprecated;
    }

    int CommunicationProxy::DispatchEvent(const std::string &sIpAddr, const std::string& sReceiver, const std::string& sSender, int iChannelId, int iEventId, const std::string& sEvent)
    {
        return McsfNotImplemnentForDeprecated;
    }

    int CommunicationProxy::CheckCastToRemote(const std::string& sName, const std::string& sIpAddr)
    {
        return McsfNotImplemnentForDeprecated;
    }

    void CommunicationProxy::SetListenAddress(const std::string& sIPAdress)
    {
        netbasecore::util::host_port_t hp(sIPAdress);
        if (hp.parsed_ok)
            m_sListenPort = hp.port;
    }

    std::string CommunicationProxy::GetListenAddress() const
    {
        return  m_pImpl ? m_pImpl->getIPsPort() : "";
    }

    std::string CommunicationProxy::GetMacAddress() const
    {
        return  m_pImpl ? m_pImpl->GetMacs() : "";
    }

    std::string CommunicationProxy::GetSystemDispatcherAddress(void) const
    {
        return m_pImpl ? m_pImpl->GetSystemDispatcherAddress() : m_sDispatcherAddr;
    }


    int CommunicationProxy::InitNetBase(int argc, char* argv[])
    {
        return McsfNotImplemnentForDeprecated;
    }

    int CommunicationProxy::SendSystemEvent(const std::string& sSender, int iEventId, const std::string& sEvent)
    {
        return m_pImpl ? m_pImpl->SendSystemEvent(sSender, m_iChannelId, iEventId, sEvent) : McsfNotCallStartListener;
    }

    int CommunicationProxy::BroadcastSystemEvent(const std::string& sSender, int iEventId, const std::string& sEvent)
    {
        return m_pImpl ? m_pImpl->BroadcastSystemEvent(sSender, m_iChannelId, iEventId, sEvent) : McsfNotCallStartListener;
    }

    void CommunicationProxy::RegisterCallBackFunction(EVENTTYPE eEvtType, boost::function1<void, void*> pFunction)
    {
    }

    void CommunicationProxy::UnregisterCallBackFunction(EVENTTYPE eEvtType)
    {
    }

}