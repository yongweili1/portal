

from McsfNetBase import ICLRCommandHandlerEx, ICLRCommandHandler, CommunicationProxy, CommandContext, \
    ICommandHandlerEx, SwigSharedCharArrayUtil_New, SwigStringUtil, \
    ISyncResult, IEventHandler, ICommandCallbackHandlerEx, ICommandCallbackHandler, IAsyncResult, SendDataContextEx,SwigSharedCharArrayUtil_Reset,\
SwigSharedCharArrayUtil_Destroy,SwigSharedCharArrayUtil_CopyToSharedArray

import ctypes
import time


class AyncCallback(ICommandCallbackHandler):
    def HandleReply(self, pAsyncResult):
        if __name__ == '__main__':
            print 'HandleReply', pAsyncResult.GetMarshalObject()
        return 0

class AyncDataCallback(ICommandCallbackHandlerEx):
    def HandleReply(self, pAsyncResult):
        print 'AyncDataCallback HandleReply,', pAsyncResult.GetMarshalObject()
        return 0

if __name__ == '__main__':
    so = ctypes.cdll.LoadLibrary
    lib = so("McsfLogger.dll")
    lib.GLogLoadConfig("log_config.xml")

    proxy = CommunicationProxy()
    proxy.SetName("front")
    proxy.CheckCastToSystemDispatcher('127.0.0.1:10000')
    proxy.StartListener("")

    cx = CommandContext()
    cx.iCommandId = 999
    cx.sReceiver = "end"
    cx.sSerializeObject = 'message_data_to_sent'

    sync_reslut = proxy.SyncSendCommand(cx)
    print '1send result', sync_reslut.GetCallResult()
    print '1response=', sync_reslut.GetSerializedObject()

    cx.iCommandId = 998

    sync_reslut = proxy.SyncSendCommand(cx)
    print '2send result', sync_reslut.GetCallResult()
    print '2response=', sync_reslut.GetSerializedObject()

    p = AyncCallback()
    cx.pCommandCallback = p

    cx.iCommandId = 999

    irtn = proxy.AsyncSendCommand(cx)
    print '3send result', irtn

    cx.iCommandId = 998

    irtn = proxy.AsyncSendCommand(cx)
    print '4send result', irtn

    proxy.SendEvent('sender', 1000, 'event_message')

    # sent = SendDataContextEx()
    # sent.sReceiver = "end"
    # t = tuple(range(10))
    # var = "huge data"
    # #
    # # bytearray
    # # SwigSharedCharArrayUtil_Reset()
    # #
    # # sent.pRawData = SwigSharedCharArrayUtil_CopyToSharedArray(var)
    # sent.sSerailizedObject = "big data sSerailizedObject"
    # sent.iWaitTime = 5000
    # dataSync = proxy.SyncSendDataEx(sent)
    # print 'dataSync.GetCallResult()=', dataSync.GetCallResult()
    # print 'dataSync.GetSerializedObject()=', dataSync.GetSerializedObject()
    #
    # a = AyncDataCallback()
    # a1 = SwigSharedCharArrayUtil_New(a)
    # sent.sSerailizedObject = "big data ****sSerailizedObject2"
    # sent.pCallback = a1
    # proxy.AsyncSendDataEx(sent)

    time.sleep(100000)
