import McsfNetBase
import time


class PyASyncCmdCallbackHandler(McsfNetBase.ICommandCallbackHandler):
    def __init__(self, p_callback_func, async_callbacks):
        McsfNetBase.ICommandCallbackHandler.__init__(self)
        self._p_callback_func = p_callback_func
        self._async_callbacks = async_callbacks
        self._async_callbacks.append(self)

    def HandleReply(self, pAsyncResult):
        # print pAsyncResult.GetMarshalObject()
        self._p_callback_func(pAsyncResult.GetMarshalObject())
        self._async_callbacks.remove(self)
        return 0


class PyCommProxy:
    def __init__(self, name, dispatcher_address, listen_address=""):
        self.cmd_handlers = {}
        self.event_handlers = {}
        self.callbacks = []
        self.proxy = McsfNetBase.CommunicationProxy()
        self.proxy.SetName(name)
        if 0 != self.proxy.CheckCastToSystemDispatcher(dispatcher_address):
            raise Exception(name, " CheckCastToSystemDispatcher failed.")
        if 0 != self.proxy.StartListener(listen_address):
            raise Exception(name, " StartListener failed.")

    def register_cmd_handler_ex(self, cmd_id, p_cmd_handler_ex):
        self.cmd_handlers[cmd_id] = p_cmd_handler_ex
        p_cmd_handler_boost = McsfNetBase.SwigSharedCharArrayUtil_New(p_cmd_handler_ex)
        return self.proxy.RegisterCommandHandlerEx(cmd_id, p_cmd_handler_boost)

    def unregister_cmd_handler_ex(self, cmd_id):
        self.proxy.UnRegisterCommandHandler(cmd_id)
        if cmd_id in self.cmd_handlers:
            del self.cmd_handlers[cmd_id]

    def register_event_handler(self, event_id, p_event_handler):
        self.event_handlers[event_id] = p_event_handler
        return self.proxy.RegisterEventHandler(0, event_id, p_event_handler)

    def unregister_event_handler(self, event_id):
        self.proxy.UnRegisterEventHandler(0, event_id)
        if event_id in self.event_handlers:
            del self.event_handlers[id]

    def sync_send_command(self, data, cmd_id, receiver, timeout=0):
        cx = McsfNetBase.CommandContext()
        cx.iCommandId = cmd_id
        cx.sReceiver = receiver
        cx.sSerializeObject = data
        cx.iWaitTime = timeout*1000
        sync_ret = self.proxy.SyncSendCommand(cx)
        return sync_ret.GetSerializedObject()

    def async_send_command(self, data, cmd_id, receiver, p_callback_func):
        cx = McsfNetBase.CommandContext()
        cx.iCommandId = cmd_id
        cx.sReceiver = receiver
        cx.sSerializeObject = data
        callback = PyASyncCmdCallbackHandler(p_callback_func, self.callbacks)
        cx.pCommandCallback = callback
        return self.proxy.AsyncSendCommand(cx)


class MyCommandHandler(McsfNetBase.ICLRCommandHandlerEx):
    def HandleCommandCLR(self, pContext):
        equation = pContext.GetSerializeObject()
        print equation
        pContext.Reply(str(sum(map(int, equation.split('+')))))


def func_cb(result):
    print 'IAsync result:', result


if __name__ == '__main__':
    fe = PyCommProxy("proxy_fe", "127.0.0.1:10000")
    be = PyCommProxy("proxy_be", "127.0.0.1:10000")
    be.register_cmd_handler_ex(10, MyCommandHandler())

    print 'Sync result:', fe.sync_send_command('1+2+3', 10, 'proxy_be')

    print 'return ', fe.async_send_command('1+2+3', 10, 'proxy_be', func_cb)

    time.sleep(10000)








