import os
import random
import sys
import threading
from netbase.comproxy import load_address
from netbase.c_log import log

from docker_manager import DockerManager


class AIServerManager(object):
    def __init__(self):
        self.docker_manager = DockerManager()
        self._instance_nm = 0
        self._evt = threading.Event()
        self._docker_list = {}

    def get_available_port(self):
        port = random.randint(15000, 25000)

        if sys.platform == 'win32':
            cmd = 'netstat -ano | findstr "{}"'.format(port)
        else:
            cmd = 'netstat -ano | grep {}'.format(port)

        desc = os.popen(cmd).read()
        return port if not desc.strip() else self.get_available_port()

    def create_algor_srv(self, gpu_id, alg_name, node_name):
        port = self.get_available_port()
        image_name = self.query_image(alg_name)
        self._instance_nm += 1
        proxy_name = '{}#{}#{}'.format(node_name, alg_name, self._instance_nm)
        cmd = 'python /seg/heart_seg.py'
        dispatcher_addr = load_address()  # as '127.0.0.1:10000'
        log_addr = log.get_address()  # as '127.0.0.1:9998'
        self.docker_manager.nvidia_run_container(dict(port=port), proxy_name, image_name, cmd, gpu_id, dispatcher_addr,
                                                 log_addr)

        self._evt.clear()
        if self._evt.wait(timeout=30):
            self._docker_list[proxy_name] = dict(srv='algor', gpu=gpu_id, proxy=proxy_name, stat=0, alg_name=alg_name)
            return proxy_name
        return ''

    def restart_a_fresh_algor_srv(self, gpu_id, alg_name, node_name):

    def handle_docker_ready(self):
        self._evt.set()

    def ribfracture_server(self):
        pass

    def registration_server(self):
        pass

    def call_server(self, gpu_id, server_name):
        self.seg_server(gpu_id)

    def query_image(self, alg_name):
        return 'heart_seg:latest'

    def to_list(self):
        for k, v in self.pro_list.items():
            yield v

if __name__ == '__main__':
    m = AIServerManager()
    n = m.get_available_port()

    print 'get available port:', n
