from McsfNetBase import ICLRCommandHandlerEx, ICLRCommandHandler, CommunicationProxy, CommandContext,\
    ICommandHandlerEx, SwigSharedCharArrayUtil_New, SwigStringUtil, ISyncResult, IEventHandler,SwigSharedCharArrayUtil_Destory,\
    IDataHandlerEx
import ctypes
import time

class CommandHandler(ICLRCommandHandler):

    def HandleCommandCLR(self, pContext, pSyncResult):
        print 'received:', pContext.sSerializeObject
        pSyncResult.SetSerializedObject('1111')
        return 0
    # def HandleCommand(self, pContext, pReplyObject):
    #     print 'received:', pContext.sSerializeObject
    #     str = "11111"
    #     pReplyObject = SwigStringUtil.ToStdString(str)
    #     return 0

class CommEx(ICLRCommandHandlerEx):
    def HandleCommandCLR(self, pContext):
        print 'received y:', pContext.GetSerializeObject()
        pContext.Reply("2222")

    # def HandleCommand(self, pContext):
    #     print 'ex received:', pContext.GetSerializeObject()
    #     pContext.Reply("response_data ex")

class Event(IEventHandler):
    def HandleEvent(self, sSender, iChannelId, iEventId, sEvent):
        print sEvent

class SyncRtn(ISyncResult):

    def __init__(self):
        self._object = ''

    def GetCallResult(self):
        return 0

    def SetSerializedObject(self, sResult):
        self._object = sResult

    def GetSerializedObject(self):
        return self._object


class BigDataHanlder(IDataHandlerEx):
    def HandleDataTrans(self, pDataContext):
        print "pDataContext.sSerailizedObject", pDataContext.sSerailizedObject
        # print "pDataContext.pRawData", pDataContext.pRawData
        pDataContext.sResponseObject = "BigDataHanlder response"
        return 0


if __name__ == '__main__':
    so = ctypes.cdll.LoadLibrary
    lib = so("McsfLogger.dll")
    lib.GLogLoadConfig("log_config.xml")

    handlers = []
    proxy1 = CommunicationProxy()
    proxy1.SetName("end")
    proxy1.CheckCastToSystemDispatcher('127.0.0.1:10000')
    proxy1.StartListener("")
    x = CommandHandler()
    proxy1.RegisterCommandHandler(999, x)
    # d = BigDataHanlder()
    # d1 = SwigSharedCharArrayUtil_New(d)
    # proxy1.RegisterDataHandlerEx(d1)

    # handlers.append(x)

    # proxy = CommunicationProxy()
    # proxy.SetName("front")
    # proxy.CheckCastToSystemDispatcher('127.0.0.1:10000')
    # proxy.StartListener("")

    # cx = CommandContext()
    # cx.iCommandId = 999
    # cx.sReceiver = "end"
    # cx.sSerializeObject = 'message_data_to_\0\n sen t'
    #
    # sync_reslut = proxy.SyncSendCommand(cx)
    # print 'send result', sync_reslut.GetCallResult()
    # print 'response=', sync_reslut.GetSerializedObject()

    y = CommEx()
    y1 = SwigSharedCharArrayUtil_New(y)
    e = Event()
    proxy1.RegisterEventHandler(0, 1000, e)
    proxy1.RegisterCommandHandlerEx(998, y1)

    # SwigSharedCharArrayUtil_Destory(y1)

    time.sleep(100000)
    del y
