import json
from netbase.cmd_event_id import CmdId


class ServiceManager(object):
    def __init__(self, net_proxy):
        self.render_srv_dict = {}
        self.algor_srv_dict = {}
        self.gpu_info_dict = {}
        self.net_proxy = net_proxy

    def register_gpu_info(self, gpu_info, sender):
        gpu = json.loads(gpu_info)
        self.gpu_info_dict[sender] = gpu

    def register_srv_list(self, srv_info, sender):
        srv_list = json.loads(srv_info)
        for srv in srv_list:
            if srv['type'] == 'algor':
                self.algor_srv_dict.get(sender, []).append(srv)
            else:
                self.render_srv_dict.get(sender, []).append(srv)

    def provide_render_srv(self):
        for srv in self.render_srv_dict.values():
            if srv['stat'] == 0:
                return srv['proxy']
        return None

    def release_render_srv(self, proxy_name):
        for srv in self.render_srv_dict.values():
            if srv['proxy'] == proxy_name:
                srv['stat'] = 0
                return True
        return False

    def provide_algor_srv(self, alg_name):
        for k, v in self.algor_srv_dict.items():
            for item in v:
                if item[''] == alg_name and item['stat'] == 0:
                    return item['proxy']

        worker_node, gpu_instance = self.__find_idle_gpu()
        if worker_node is not None:
            return self.__run_fresh_algor_srv(worker_node, gpu_instance, alg_name)

        worker_node, gpu_instance = self.__find_a_free_gpu()
        if worker_node is not None:
            return self.__restart_fresh_algor_srv(worker_node, gpu_instance, alg_name)

        return ''

    def __find_idle_gpu(self):
        for work_node, gpu in self.gpu_info_dict:
            gpu_number = gpu['number']
            alg_srv_list = self.algor_srv_dict[work_node]
            gpu_used = []
            for srv in alg_srv_list:
                gpu_used.append(srv['gpu'])
            for i in range(gpu_number):
                if i not in gpu_used:
                    return work_node, i

        return None

    def __find_a_free_gpu(self):
        for wrk_node, gpu_list in self.algor_srv_dict.items():
            for item in gpu_list:
                if item['stat'] == 0:
                    return wrk_node, item['gpu']
        return None

    def reset_gpu_stat(self, sender, gpu_instance):
        for wrk_node, gpu_list in self.algor_srv_dict.items():
            if wrk_node == sender:
                for item in gpu_list:
                    if item['gpu'] == gpu_instance:
                        item['stat'] = 0
                        return True
        return False

    def __restart_fresh_algor_srv(self, worker_node, gpu, alg_name):
        data = dict(gpu=gpu, alg_name=alg_name)
        return self.net_proxy.sync_send_command(json.dumps(data), CmdId.cmd_id_restart_algor_srv, worker_node)

    def __run_fresh_algor_srv(self, worker_node, gpu, alg_name):
        data = dict(gpu=gpu, alg_name=alg_name)
        return self.net_proxy.sync_send_command(json.dumps(data), CmdId.cmd_id_run_algor_srv, worker_node)


if __name__ == "__main__":
    d = dict(gpu=1, alg_name='seg')
    s = json.dumps(d)
    o = json.loads(s)
    print o['gpu'], o['alg_name']
