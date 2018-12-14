#-*-coding:utf-8-*-

from docker_manager import DockerManager

docker_manager = DockerManager()

def alg_srv_ready(p_context):
    awake()

class AIServerManager(object):
    def query_port(self):
        return '8989'

    def create_algor_srv(self, gpu_id, alg_name):
        port = self.query_port()
        image_name = self.query_image(alg_name)
        cmd = 'python /seg/heart_seg.py'
        docker_manager.nvidia_run_container(dict(port=port), proxy_name, image_name, cmd, gpu_id)

        wait()
        return  proxy_name

    def ribfracture_server(self):
        pass

    def registration_server(self):
        pass

    def call_server(self, gpu_id, server_name):
            self.seg_server(gpu_id)

    def query_image(self, alg_name):
        return 'heart_seg:latest'
