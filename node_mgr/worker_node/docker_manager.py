#-*-coding:utf-8-*-

import docker
import subprocess
import time


class DockerManager(object):

    def __init__(self):
        self.client = docker.from_env()
        self.attr = {}

    def find_all_container(self):
        all_container_id = []
        containers = self.client.containers.list(all=True)
        for container in containers:
            all_container_id.append(container.short_id)
        print all_container_id
        return all_container_id
        # return containers

    def find_on_container(self):
        on_container_id = []
        containers = self.client.containers.list()
        for container in containers:
            on_container_id.append(container.short_id)
        print "Running Container : {}".format(on_container_id)
        return on_container_id

    def container_attribute(self, container_id):
        """

        :param container_id: the container_id you want to find the attribute
        :return:
        """
        for item in self.client.containers.list(all=True):
            if item.short_id == container_id:
                self.attr['ContainerId'] = container_id
                self.attr['cmd'] = item.attrs['Config']['Cmd']
                self.attr['State'] = item.attrs['State']['Status']
                self.attr['Start_time'] = item.attrs['State']['StartedAt']
                self.attr['End_time'] = item.attrs['State']['FinishedAt']
                self.attr['Image'] = item.image.tags
                self.attr['ImageID'] = item.image.id
                self.attr['IP'] = item.attrs['NetworkSettings']['IPAddress']
                self.attr['MacAddress'] = item.attrs['NetworkSettings']['MacAddress']
                self.attr['Port'] = item.attrs['NetworkSettings']['Ports'].values()[0][0]['HostPort']
        print self.attr

        return self.attr

    def container_status(self, container_id):
        for item in self.client.containers.list(all=True):
            if item.short_id == container_id:
                print str(item.status)

    def start_container(self, container_id):
        for item in self.client.containers.list(all=True):
            if item.short_id == container_id:
                item.start()

    def stop_container(self, server_name):
        # for item in self.client.containers.list(all=True):
        #     if item.short_id == container_id:
        #         item.stop()

        for item in self.client.containers.list(all=True):
            if item.name == server_name:
                item.stop()

    def remove_container(self, server_name):
        for item in self.client.containers.list(all=True):
            if item.name == server_name:
                item.remove()

    def create_image(self, dockerfile_dir, tag):
        """

        :param dockerfile_dir: str, file contain the dockerfile
        :param tag: str, the image name
        :return: image object
        """
        return self.client.images.build(path=dockerfile_dir, tag=tag, rm=True)

    def run_container(self, ports, image_name, cmd, name, detach=False):
        """

        :param image_name: str, image name
        :param ports: dict, 'port':port
        :param name: str, The name for this container
        :param detach: default False
        :return: container object
        """
        return self.client.containers.run(image_name, command=cmd,  name=name, runtime='nvidia')

    def nvidia_run_container(self, port, image_name, cmd, gpu_id, container_name='', backstage=True):
        """

        :param port: str
        :param image_name: str
        :param cmd: the shell you wanted to run when the container started
        :param gpu_id: int
        :param container_name: The name for this container
        :param backstage:  default run in backstage
        :return:
        """
        sub_cmd = 'nvidia-docker run -p={0} -d={1} --name={2} {3} {4} {5}'\
            .format(port, backstage, container_name, image_name, cmd, gpu_id)
        print sub_cmd
        p = subprocess.Popen(sub_cmd, stdout=subprocess.PIPE, shell=True, close_fds=True)
        p.communicate()
        return p.returncode


if __name__ == "__main__":
    import nvdocker
    a = DockerManager()

    #start and stop container
    # b = a.find_all_container()
    # c = a.find_on_container()
    #
    for i in range(5):
        a.stop_container('heart_seg')
        a.find_on_container()
        a.start_container('heart_seg')
        a.find_on_container()
    #
    # a.container_status(c[0])
    # a.container_attribute(c[0])

    #create image
    # dockerfile_path = '/home/lyw/composeapp'
    # print time.asctime(time.localtime(time.time()))
    # image = a.create_image(dockerfile_path, tag='ksjfkds')
    # print time.asctime(time.localtime(time.time()))

    # nvidia-run container
    # port = '3004:3004'
    # image = 'heart_seg:latest'
    # cmd = 'python /seg/seg_al.py 3'
    # gpu_id = 3
    # host_dir = '/home/lyw/seg_result'
    # container_name = 'uAI_1125'
    # # a.run_container(port, image, cmd, container_name)
    # a.nvidia_run_container(port, image, cmd, gpu_id, host_dir, container_name)

