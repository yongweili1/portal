import json
import threading

from command import command as cmd
from message import RequestData

import struct

from netbase import comproxy
from netbase import c_log


import time


class MyCommandHandler(comproxy.PyBaseCmdHandlerEx):
    def __init__(self):
        comproxy.PyBaseCmdHandlerEx.__init__(self)

    def handle_command(self, p_context):
        current_package_data = p_context.GetSerializeObject()
        data = RequestData(current_package_data)
        b = time.time()

        rsp = cmd.commands[data.command](**data.kwargs)
        c = time.time()
        p_context.Reply(rsp)
        print('handler use{} ms'.format(str((c - b) * 1000)))


if __name__ == '__main__':
    # c_log.PyLogInstance().create_log()
    proxy = comproxy.PyCommProxy("img_srv", "10.9.19.150:10000")
    x = MyCommandHandler()
    proxy.register_cmd_handler_ex(100, x)

    e = threading.Event()
    e.wait()

