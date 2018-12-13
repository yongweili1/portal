import json


class ServiceManager(object):
    def __init__(self, net_proxy):
        self.render_srv_list = {}
        self.algor_srv_list = {}
        self.gpu_info = {}
        self.net_proxy = net_proxy

    def handle_gpu_info_report(self, gpu_info, sender):
        gpu = json.loads(gpu_info)
        self.gpu_info[sender] = gpu

    def handle_srv_list_report(self, srv_info, sender):
        srv_list = json.loads(srv_info)
        self.render_srv_list[sender] = srv_list

    def handle_find_free_algor_srv(self, alg_name):
        for k, v in self.algor_srv_list.items():
            for item in v:
                if item[''] == alg_name and item['stat'] == 0:
                    return item['proxy']

        worker_node, gpu_instance = self.__find_a_free_gpu()
        return self.__restart_fresh_algor(worker_node, gpu_instance)

    def __find_a_free_gpu(self):
        pass

    def __restart_fresh_algor(self, worker_node, gpu_instance):

    def handle_find_free_render_srv(self):
        pass

    def handle_release_render_srv(self, proxy_name):
        pass
