import Queue
import os
import sys
import threading
import time
import xml.etree.ElementTree as XmlEt

from c_log import log

if sys.platform == 'win32':
    import netbase_win32 as c_net_base
else:
    import netbase_unix as c_net_base


class WorkingThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.Queue = Queue.Queue(0)

    def run(self):
        while True:
            f, parameters = self.Queue.get(True)
            f(parameters)
            self.Queue.task_done()

    def post(self, f, parameters):
        self.Queue.put_nowait((f, parameters))


class PyCommandContextEx(object):
    def __init__(self, p_context):
        # add reference
        self._command_context = p_context.Clone()

    def __delete__(self, instance):
        self._command_context.Destroy()

    def get_receiver(self):
        return self._command_context.GetReceiver()

    def get_sender(self):
        return self._command_context.GetSender()

    def get_command_id(self):
        return self._command_context.GetCommandId()

    def get_serialize_object(self):
        return self._command_context.GetSerializeObject()

    def reply(self, reply_object):
        return self._command_context.Reply(reply_object)


class PyCommonHandler(c_net_base.ICLRCommandHandlerEx, c_net_base.IEventHandler):
    def __init__(self, thread):
        c_net_base.ICLRCommandHandlerEx.__init__(self)
        c_net_base.IEventHandler.__init__(self)
        self._thread = thread
        self._handlers = {}

    def register_handler(self, uid, handler):
        self._handlers[uid] = handler
        return 0

    def remove_handler(self, uid):
        if uid in self._handlers:
            del self._handlers[uid]

    def HandleCommandCLR(self, pContext):
        uid = pContext.GetCommandId()
        if uid in self._handlers:
            self._thread.post(self._handlers[uid], PyCommandContextEx(pContext))
        return 0

    def HandleEvent(self, sSender, iChannelId, iEventId, sEvent):
        if iEventId in self._handlers:
            self._thread.post(self._handlers[iEventId], sEvent)
        return 0


class PyAsyncResult(object):
    def __init__(self, p_async_result):
        self._async_result = p_async_result.Clone()

    def __delete__(self, instance):
        self._async_result.Destory()

    def get_marshal_object(self):
        return self._async_result.GetMarshalObject()


class PyASyncCmdCallbackHandler(c_net_base.ICommandCallbackHandler):
    def __init__(self, p_callback_func, async_callbacks, thread):
        c_net_base.ICommandCallbackHandler.__init__(self)
        self._p_callback_func = p_callback_func
        self._async_callbacks = async_callbacks
        self._async_callbacks.append(self)
        self._thread = thread

    def handle_reply(self, p_async_result):
        self._p_callback_func(p_async_result.get_marshal_object())
        self._async_callbacks.remove(self)

    def HandleReply(self, pAsyncResult):
        self._thread.post(self.handle_reply, PyAsyncResult(pAsyncResult))
        return 0


def load_address():
    path = os.path.dirname(os.path.abspath(__file__))
    xml_path = os.path.join(path, 'config/dispatcher_config.xml')

    root = XmlEt.parse(xml_path).getroot()
    return root.find('IP').text + ':' + root.find('Port').text


class PyCommProxy:
    def __init__(self, name, listen_address="", dispatcher_address=""):
        self._thread = WorkingThread()
        self._thread.start()
        self._handler = PyCommonHandler(self._thread)
        self.callbacks = []
        self.proxy = c_net_base.CommunicationProxy()
        self.proxy.SetName(name)
        if not dispatcher_address.strip():
            dispatcher_address = load_address()
        if not dispatcher_address.strip():
            dispatcher_address = "127.0.0.1:10000"
        if 0 != self.proxy.CheckCastToSystemDispatcher(dispatcher_address):
            raise Exception(name, " CheckCastToSystemDispatcher failed.")
        if 0 != self.proxy.StartListener(listen_address):
            raise Exception(name, " StartListener failed.")

    def register_cmd_handler(self, cmd_id, p_cmd_handler):
        self._handler.register_handler(cmd_id, p_cmd_handler)
        p_cmd_handler_boost = c_net_base.SwigSharedCharArrayUtil_New(self._handler)
        return self.proxy.RegisterCommandHandlerEx(cmd_id, p_cmd_handler_boost)

    def unregister_cmd_handler(self, cmd_id):
        self.proxy.UnRegisterCommandHandler(cmd_id)
        self._handler.remove_handler(cmd_id)

    def register_event_handler(self, event_id, p_event_handler):
        self._handler.register_handler(event_id, p_event_handler)
        return self.proxy.RegisterEventHandler(0, event_id, self._handler)

    def unregister_event_handler(self, event_id):
        self.proxy.UnRegisterEventHandler(0, event_id)
        self._handler.remove_handler(event_id)

    def sync_send_command(self, data, cmd_id, receiver, timeout=0):
        cx = c_net_base.CommandContext()
        cx.iCommandId = cmd_id
        cx.sReceiver = receiver
        cx.sSerializeObject = data
        cx.iWaitTime = timeout*1000
        sync_ret = self.proxy.SyncSendCommand(cx)
        return sync_ret.GetSerializedObject()

    def async_send_command(self, data, cmd_id, receiver, p_callback_func):
        cx = c_net_base.CommandContext()
        cx.iCommandId = cmd_id
        cx.sReceiver = receiver
        cx.sSerializeObject = data
        callback = PyASyncCmdCallbackHandler(p_callback_func, self.callbacks, self._thread)
        cx.pCommandCallback = callback
        return self.proxy.AsyncSendCommand(cx)

    def send_event(self, data, event_id):
        return self.proxy.SendEvent('', event_id, data)


def async_func_cb(result):
    print 'IAsync result:', result


def command_handler(p_context):
        equation = p_context.get_serialize_object()
        print equation
        p_context.reply(str(sum(map(int, equation.split('+')))))


def event_handler(s_event):
    print 'handle_event, ' + s_event


if __name__ == '__main__':
    log.create_log()
    fe = PyCommProxy("proxy_fe")
    be = PyCommProxy("proxy_be")
    be.register_cmd_handler(10, command_handler)
    be.register_event_handler(11, event_handler)

    print 'Sync result:', fe.sync_send_command('1+2+3', 10, 'proxy_be')

    print 'return ', fe.async_send_command('1+2+3', 10, 'proxy_be', async_func_cb)

    fe.send_event('event broadcast!', 11)

    time.sleep(10000)








