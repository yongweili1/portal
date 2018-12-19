import json
import threading
import time
import uuid

from netbase.c_log import log
from netbase import comproxy
from netbase.cmd_event_id import CmdId
from ai_server_mgr import AIServerManager
from process_manager import RenderManager


def get_mac_address():
    mac = uuid.UUID(int=uuid.getnode()).hex[-12:]
    return ":".join([mac[el:el + 2] for el in range(0, 11, 2)])


if __name__ == '__main__':
    log.create_log()
    log.set_source('WorkNode')
    log.dev_info('first log of work node on {}'.format(get_mac_address()))

    proxy = comproxy.PyCommProxy('work_node_{}'.format(get_mac_address()))
    render_srv_manager = RenderManager(proxy)
    render_srv_manager.create_srv(proxy.get_name(), 4)

    ai_srv_mgr = AIServerManager()
    #ai_srv_mgr.create_algor_srv(0, 'auto_segment', proxy.get_name())

    def handle_run_algor_srv(p_context):
        info = json.loads(p_context.get_serialize_object())
        gpu_id = info['gpu']
        alg_name = info['alg_name']
        if p_context.get_command_id() == CmdId.cmd_id_run_algor_srv:
            proxy_name = ai_srv_mgr.create_algor_srv(gpu_id, alg_name, proxy.get_name())
        else:
            proxy_name = ai_srv_mgr.restart_a_fresh_algor_srv(gpu_id, alg_name, proxy.get_name())
        p_context.reply(proxy_name)


    proxy.register_cmd_func(CmdId.cmd_id_run_algor_srv, handle_run_algor_srv)
    proxy.register_cmd_func(CmdId.cmd_id_restart_algor_srv, handle_run_algor_srv)

    def handle_docker_ready(p_context):
        ai_srv_mgr.handle_docker_ready()
        p_context.reply(str(True))

    proxy.register_cmd_func(CmdId.cmd_id_process_ready, handle_docker_ready)

    # report to center_node gpu info and current svr_list
    def get_gpu_info():
        return json.dumps(dict(number=4, memory=1024))

    const_center_node = 'center_node'
    while not proxy.sync_send_command(get_gpu_info(), CmdId.cmd_id_report_gpu_info, const_center_node).strip():
        time.sleep(1)
        log.dev_info('can not find center_node')

    def get_current_srv_list():
        srv_list = []
        for x in render_srv_manager.to_list():
            srv_list.append(x)

        for x in ai_srv_mgr.to_list():
            srv_list.append(x)
        return json.dumps(srv_list)

    reply = proxy.sync_send_command(get_current_srv_list(), CmdId.cmd_id_report_srv_info, const_center_node)
    log.dev_info('report srv info reply {}'.format(reply))
    threading.Event().wait()

