import random
import time

from netbase.c_log import log
from netbase.comproxy import PyCommProxy
from netbase.cmd_event_id import CmdId

if __name__ == '__main__':
    log.create_log()

    proxy = PyCommProxy('test_node')

    while True:
        x = random.randint(1, 4)
        names = []
        print 'ready to apply {} render server.'.format(x)
        for i in range(x):
            names.append(proxy.sync_send_command('', CmdId.cmd_id_find_render_srv, 'center_node'))
        print 'apply ', names
        for n in names:
            proxy.sync_send_command(n, CmdId.cmd_id_release_render_srv, 'center_node')
        print 'release ', names

        time.sleep(10)


