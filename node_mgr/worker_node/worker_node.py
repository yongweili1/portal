import json
import threading
import uuid


from netbase.c_log import log
from netbase import comproxy
from netbase.cmd_event_id import CmdId
from gpu_manager import GpuManager
from ai_server import AIServer


gpu_manager = GpuManager()


def get_gpu_info():
    free_gpu_id = gpu_manager.get_gpu_memory_infos()
    return json.dumps(free_gpu_id)
    # return json.dumps({'number': 4, 'memory': 1024})


def run_render_srv(number_to_run):
    pass


def run_algor_srv(gpu_id, server_name):
    return AIServer().call_server(gpu_id, server_name)


def restart_a_fresh_algor_srv(gpu_instance, alg_name):
    kill()
    run_algor_srv
    return 'proxy_name'


def start_initial_srv():
    run_render_srv(4)
    run_algor_srv()


def get_current_srv_list():
    srv_list = [{'srv': 'render', 'gpu': -1, 'proxy': 'img_srv#0', 'stat': 0},
                {'srv': 'render', 'gpu': -1, 'proxy': 'img_srv#1', 'stat': 0},
                {'srv': 'algor', 'gpu': 0, 'proxy': 'auto_segment#0', 'stat': 0, 'alg_name': 'auto_segment'}]

    return json.dumps(srv_list)


def handle_restart_algor_srv(p_context):
    content = p_context.get_serialize_object()
    req = json.loads(content)
    proxy_name = restart_a_fresh_algor_srv(req['gpu'], req['alg_name'])
    p_context.reply(proxy_name)


def get_mac_address():
    mac = uuid.UUID(int=uuid.getnode()).hex[-12:]
    return ":".join([mac[el:el + 2] for el in range(0, 11, 2)])




if __name__ == '__main__':
    log.create_log()
    log.set_source('WorkNode')

    proxy = comproxy.PyCommProxy('work_node_{}'.format(get_mac_address()), ':4200', '10.9.19.153:10000')
    proxy.register_cmd_handler(CmdId.cmd_id_restart_algor_srv, handle_restart_algor_srv)

    start_initial_srv()

    proxy.sync_send_command('')

    const_center_node = 'center_node'
    reply = proxy.sync_send_command(get_gpu_info(), CmdId.cmd_id_report_gpu_info, const_center_node)
    log.dev_info('report gpu info reply {}'.format(reply))

    reply = proxy.sync_send_command(get_current_srv_list(), CmdId.cmd_id_report_srv_info, const_center_node)
    log.dev_info('report srv info reply {}'.format(reply))
    threading.Event().wait()

