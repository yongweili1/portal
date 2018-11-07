#include "mcsf_communication_proxy_impl.h"


class testEventHandler : public Mcsf::IEventHandler
{
    virtual int HandleEvent(const std::string& sSender, int iChannelId, int iEventId, const std::string& sEvent)
    {
        printf("recv event : %s\n", sEvent.c_str());
        return 0;
    }
};

class commandHandler : public Mcsf::ICommandHandler
{
    virtual int HandleCommand(const Mcsf::CommandContext* pContext, std::string* pReplyObject)
    {
        printf("commandHandler recv from %s data=%s\n", pContext->sSender.c_str(), pContext->sSerializeObject.c_str());
        *pReplyObject = pContext->sSerializeObject;
        *pReplyObject += "afff";
        return 0;
    }
};

class commandHandlerEx :public Mcsf::ICommandHandlerEx
{
    virtual void HandleCommand(boost::shared_ptr<Mcsf::ICommandContextEx> pContext) {
        printf("commandHandlerEx recv from %s data=%s\n", pContext->GetSender().c_str(), pContext->GetSerializeObject().c_str());
        pContext->Reply(pContext->GetSerializeObject());
    }
};

class callbackHandler : public Mcsf::ICommandCallbackHandler
{
    virtual int HandleReply(const Mcsf::IAsyncResult* pAsyncResult)
    {
        printf("HandleReply data=%s\n", pAsyncResult->GetMarshalObject().c_str());
        return 0;
    }
};

class dataHandler : public Mcsf::IDataHandlerEx
{
    virtual void HandleDataTrans(boost::shared_ptr<Mcsf::SendDataContextEx> pDataContext)
    {
        printf("recv blob len=%d, obj=%s\n", pDataContext->iLen, pDataContext->sSerailizedObject.c_str());
        pDataContext->sResponseObject = "reply";
    }
};

class dataCallbk :public Mcsf::ICommandCallbackHandlerEx
{
    void HandleReply(boost::shared_ptr<Mcsf::IAsyncResult> pAsyncResult) {
        printf("recv ayncEx blob rep=%s, callret=%d\n", pAsyncResult->GetMarshalObject().c_str(), pAsyncResult->GetCallResult());
    }
};

int main(int argc, char* argv[])
{
    Mcsf::CommunicationProxyImpl::ImplEntry env;
    env.dispatcherAddr = "127.0.0.1:10000";
    env.handleThreNum = 1;
    env.ioThrNum = 1;
    env.listenPort = "";
    env.name = "CommunicationProxyImpl";

    Mcsf::CommunicationProxyImpl * impl = nullptr;
    try
    {
        impl = new Mcsf::CommunicationProxyImpl(env);
    }
    catch (std::exception &e)
    {
        printf(e.what());
        return -1;
    }

    impl->RegisterCommandHandler(100, new commandHandler);
    boost::shared_ptr<Mcsf::ICommandHandlerEx> ex(new commandHandlerEx);
    impl->RegisterCommandHandlerEx(101, ex);

    impl->RegisterEventHandler(0, 102, new testEventHandler);

    impl->RegisterDataHandlerEx(boost::shared_ptr<Mcsf::IDataHandlerEx>(new dataHandler()));


    Mcsf::CommunicationProxyImpl *impl2 = nullptr;
    try
    {
        Mcsf::CommunicationProxyImpl::ImplEntry e(env);
        e.name = "com2";
        impl2 = new Mcsf::CommunicationProxyImpl(e);
    }
    catch (std::exception &e)
    {
        printf(e.what());
        return -1;
    }

    for (;;)
    {


        Mcsf::SendDataContextEx dataEx;
        dataEx.pRawData.reset(new char[1024 * 10]);
        dataEx.iLen = 1024 * 10;
        dataEx.iWaitTime = 1000;
        dataEx.sSender = impl2->GetName();
        dataEx.sReceiver = impl->GetName();
        dataEx.sSerailizedObject = "object";
        std::string datatrep;
        impl2->SyncSendDataEx(dataEx, datatrep);

        dataEx.pCallback.reset(new dataCallbk);
        impl2->AsyncSendDataEx(dataEx);

        impl2->SendEvent(env.name, 102, "send_event");
        impl2->BroadcastEvent(env.name, 102, "broadcast_event");

        Mcsf::CommandContext c1;
        c1.iCommandId = 100;
        c1.sReceiver = env.name;
        c1.iWaitTime = 0;
        c1.sSerializeObject = "test_cmd";
        c1.pCommandCallback = new callbackHandler;

        std::string rep;
        int irtn = impl2->SyncSendCommand(&c1, rep);

        c1.sSerializeObject = "async test_cmd";
        irtn = impl2->AsyncSendCommand(&c1);

        c1.iCommandId = 101;
        c1.sSerializeObject = "sync test_cmd_ex_1";
        irtn = impl2->SyncSendCommand(&c1, rep);
        c1.sSerializeObject = "async test_cmd_ex_2";
        irtn = impl2->AsyncSendCommand(&c1);


        boost::this_thread::sleep(boost::posix_time::millisec(100));

    }
    boost::this_thread::sleep(boost::posix_time::seconds(100000));
    return 0;
}