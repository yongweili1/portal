import Queue
import threading

from netbase import comproxy
from netbase.c_log import log
from netbase.cmd_event_id import CmdId,EventId
import srv_manager


class WorkingThread(threading.Thread):
    def __init__(self, srv_mgr):
        threading.Thread.__init__(self)
        self.evt = threading.Event()
        self.Queue = Queue.Queue(0)
        self.srv_manager = srv_mgr

    def run(self):
        while True:
            p_context = self.Queue.get(True)
            while True:
                name = self.srv_manager.provide_algor_srv(p_context.get_serialize_object())
                if 0 == len(name):
                    self.evt.clear()
                    self.evt.wait()
                else:
                    p_context.reply(name)
                    break

            self.Queue.task_done()

    def post(self, p_context):
        self.Queue.put_nowait(p_context)

    def wake(self):
        self.evt.set()


if __name__ == '__main__':
    log.create_log()
    log.set_source('CenterNode')

    proxy = comproxy.PyCommProxy('center_node')

    srv_manager = srv_manager.ServiceManager(proxy)

    work_thread = WorkingThread(srv_manager)
    work_thread.start()

    def handle_find_algor_srv(p_context):
        work_thread.post(p_context)

    def handle_find_render_srv(p_context):
        # TODO: post to work_thread
        name = srv_manager.provide_render_srv()
        p_context.reply(str(name))

    def handle_report_gpu(p_context):
        srv_manager.register_gpu_info(p_context.get_serialize_object(), p_context.get_sender())
        p_context.reply(str(True))

    def handle_report_svr_list(p_context):
        srv_manager.register_srv_list(p_context.get_serialize_object(), p_context.get_sender())
        p_context.reply(str(True))

    def handle_release_render_srv(p_context):
        ret = srv_manager.release_render_srv(p_context.get_serialize_object())
        p_context.reply(str(ret))
        work_thread.wake()

    def handle_algor_srv_free(event_dict):
        srv_manager.reset_gpu_stat(event_dict['sender'], int(event_dict['data']))
        work_thread.wake()

    def handle_srv_dead(event_dict):
        srv_manager.erase_srv(event_dict['data'], event_dict['sender'])


    proxy.register_cmd_func(CmdId.cmd_id_find_algor_srv, handle_find_algor_srv)
    proxy.register_cmd_func(CmdId.cmd_id_report_gpu_info, handle_report_gpu)
    proxy.register_cmd_func(CmdId.cmd_id_report_srv_info, handle_report_svr_list)
    proxy.register_cmd_func(CmdId.cmd_id_find_render_srv, handle_find_render_srv)
    proxy.register_cmd_func(CmdId.cmd_id_release_render_srv, handle_release_render_srv)
    proxy.register_event_func(EventId.event_id_broadcast_free, handle_algor_srv_free)
    proxy.register_event_func(EventId.event_id_broadcast_srv_dead, handle_srv_dead)

    threading.Event().wait()
