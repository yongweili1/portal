import threading

from netbase import comproxy
from netbase.c_log import log
from netbase.cmd_event_id import CmdId
import srv_manager


if __name__ == '__main__':
    log.create_log()
    log.set_source('CenterNode')

    proxy = comproxy.PyCommProxy('center_node')

    srv_manager = srv_manager.ServiceManager(proxy)

    def handle_find_algor_srv(p_context):
        name = srv_manager.provide_algor_srv(p_context.get_serialize_object())
        p_context.reply(name)

    def handle_find_render_srv(p_context):
        name = srv_manager.provide_render_srv()
        p_context.reply(name)

    def handle_report_gpu(p_context):
        srv_manager.register_gpu_info(p_context.get_serialize_object(), p_context.get_sender())
        p_context.reply(str(True))

    def handle_report_svr_list(p_context):
        srv_manager.register_srv_list(p_context.get_serialize_object(), p_context.get_sender())
        p_context.reply(str(True))

    def handle_release_render_srv(p_context):
        ret = srv_manager.release_render_srv(p_context.get_serialize_object())
        p_context.reply(str(ret))

    def handle_algor_srv_free(event_dict):
        srv_manager.reset_gpu_stat(event_dict['sender'], int(event_dict['data']))


    proxy.register_cmd_handler(CmdId.cmd_id_find_algor_srv, handle_find_algor_srv)
    proxy.register_cmd_handler(CmdId.cmd_id_report_gpu_info, handle_report_gpu)
    proxy.register_cmd_handler(CmdId.cmd_id_report_srv_info, handle_report_svr_list)
    proxy.register_cmd_handler(CmdId.cmd_id_find_render_srv, handle_find_render_srv)
    proxy.register_cmd_handler(CmdId.cmd_id_release_render_srv, handle_release_render_srv)

    threading.Event().wait()
