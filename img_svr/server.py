import os
import sys
import threading
import time

from netbase.c_log import log
log.create_log()
log.set_source('img_srv')
log.dev_info('first log of img_srv.')

from command import command as cmd
from message import RequestData
from netbase import comproxy
from netbase.cmd_event_id import CmdId


def command_func(p_context):
    current_package_data = p_context.get_serialize_object()
    data = RequestData(current_package_data)
    b = time.time()

    rsp = cmd.commands[data.command](**data.content)
    c = time.time()
    p_context.reply(rsp)
    log.dev_info('handler use {} ms'.format(str((c - b) * 1000)))


def command_load_volume(p_context):
    current_package_data = p_context.get_serialize_object()
    root = os.path.dirname(os.path.abspath(__file__)).split('portal')[0]
    root = os.path.join(root, 'portal_ferry')
    dir_path = os.path.join(root, 'srv/volume')
    if not os.path.isdir(dir_path):
        os.makedirs(dir_path)
    volumepath = dir_path + r'\image.nii.gz'
    with open(volumepath, 'wb') as f:
        f.write(current_package_data)
    rsp = cmd.commands['load'](volumepath=volumepath)
    p_context.reply(rsp)


if __name__ == '__main__':
    proxy_name = 'img_srv'
    if len(sys.argv) > 1:
        proxy_name = sys.argv[1]

    proxy = comproxy.PyCommProxy(proxy_name)
    proxy.register_cmd_func(CmdId.cmd_id_render, command_func)
    proxy.register_cmd_func(CmdId.cmd_id_load_volume, command_load_volume)
    log.dev_info('register_cmd_handler {}'.format(CmdId.cmd_id_render))
    threading.Event().wait()
