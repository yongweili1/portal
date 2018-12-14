import sys
import threading

from netbase.c_log import log
from netbase import comproxy
from netbase.cmd_event_id import CmdId, EventId

if __name__ == '__main__':
    proxy_name = sys.argv[1]
    port = sys.argv[2]
    dispatcher_addr = sys.argv[3]
    gpu_id = sys.argv[4]
    parent_node_name = sys.argv[5]

    log.create_log()
    log.set_source('WorkNode')

    proxy = comproxy.PyCommProxy(proxy_name, port, dispatcher_addr)


    def handle_algor_task(p_context):
        data = p_context.get_serialize_object()
        path = save(data)

        seg_content = segment(path)
        p_context.reply(seg_content)
        os.remove(path)
        proxy.send_event(str(gpu_id), EventId.event_id_broadcast_free)


    proxy.register_cmd_handler(CmdId.cmd_id_restart_algor_srv, handle_algor_task)

    proxy.sync_send_command('', CmdId.cmd_id_process_ready, parent_node_name)

    threading.Event().wait()