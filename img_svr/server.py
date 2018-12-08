import threading
import time

from netbase.c_log import log
log.create_log()
log.set_source('img_srv')
log.dev_info('first log of img_srv.')

from command import command as cmd
from message import RequestData
from netbase import comproxy


def command_handler(p_context):
    current_package_data = p_context.get_serialize_object()
    data = RequestData(current_package_data)
    b = time.time()

    rsp = cmd.commands[data.command](**data.content)
    c = time.time()
    p_context.reply(rsp)
    log.dev_info('handler use {} ms'.format(str((c - b) * 1000)))


if __name__ == '__main__':
    proxy = comproxy.PyCommProxy("img_srv", "127.0.0.1:10000")
    proxy.register_cmd_handler(100, command_handler)
    log.dev_info('register_cmd_handler 100')
    e = threading.Event()
    e.wait()
