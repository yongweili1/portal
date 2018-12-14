#-*-coding:utf-8-*-

from docker_manager import DockerManager

docker_manager = DockerManager()


class AIServer(object):

    def seg_server(self, gpu_id):
        port = '8989:8989'
        image_name = 'heart_seg:latest'
        cmd = 'python /seg/heart_seg.py'
        docker_manager.nvidia_run_container(port, image_name, cmd, gpu_id)

    def ribfracture_server(self):
        pass

    def registration_server(self):
        pass

    def call_server(self, gpu_id, server_name):
            self.seg_server(gpu_id)
