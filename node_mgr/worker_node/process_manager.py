import json
import os

from netbase.c_log import log
from netbase.cmd_event_id import EventId
import subprocess
import threading
import time


class RenderManager(threading.Thread):
    def __init__(self, proxy):
        threading.Thread.__init__(self)
        self.pro_list = {}
        self.pro_name_index = 0
        self._stopped = False
        self.proxy = proxy

    def create_srv(self, node_name, count):
        cwd = os.path.abspath(__file__)
        cwd = cwd.split('portal')[0]
        cwd = os.path.join(cwd, 'portal', 'img_svr')

        for x in range(count):
            self.pro_name_index += 1

            proxy_name = "{}#img_srv#{}".format(node_name, self.pro_name_index)
            cmd_line = "python server.py {}".format(proxy_name)

            child = subprocess.Popen(cmd_line, shell=False, cwd=cwd)
            print '---{}---'.format(child.pid)
            self.pro_list[proxy_name] = child

    def kill_srv(self, proxy_name):
        if proxy_name in self.pro_list:
            self.pro_list[proxy_name].terminate()
            self.pro_list.pop(proxy_name)

    def kill_all(self):
        for prc in self.pro_list.values():
            prc.terminate()

        self.pro_list.clear()

    def stop(self):
        self.kill_all()
        self._stopped = True
        self.join()

    def run(self):
        while not self._stopped:
            for k, x in self.pro_list.items():
                if x.poll() is not None:
                    print '{} is dead! return {}'.format(k, x.returncode)
                    log.dev_error('{} is dead! return {}'.format(k, x.returncode))
                    self.proxy.send_event(json.dumps(srv='render', proxy=k), EventId.event_id_broadcast_srv_dead)
                    self.pro_list.pop(k)

            time.sleep(1)

    def to_list(self):
        for k in self.pro_list.keys():
            yield dict(srv='render', gpu=-1, proxy=k, stat=0)


if __name__ == '__main__':
    proc_mgr = RenderManager()
    proc_mgr.start()

    proc_mgr.create_srv('11', 4)

    time.sleep(10)
    proc_mgr.kill_all()
    proc_mgr.stop()
