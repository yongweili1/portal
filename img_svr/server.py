import threading
import time

from command import command as cmd
from message import RequestData
from netbase import comproxy
from netbase.c_log import log


def command_handler(p_context):
    current_package_data = p_context.get_serialize_object()
    data = RequestData(current_package_data)
    b = time.time()

    rsp = cmd.commands[data.command](**data.content)
    c = time.time()
    p_context.reply(rsp)
    print('handler use{} ms'.format(str((c - b) * 1000)))


if __name__ == '__main__':
    log.create_log()
    proxy = comproxy.PyCommProxy("img_srv", "127.0.0.1:10000")
    proxy.register_cmd_handler(100, command_handler)
    e = threading.Event()
    e.wait()
