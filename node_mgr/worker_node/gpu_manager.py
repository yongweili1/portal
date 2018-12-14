#-*-coding:utf-8-*-

from pynvml import *


class GpuManager(object):
    def __init__(self):
        self.init = nvmlInit()

    def get_gpu_memory_infos(self):
        gpu_info = {}
        device_count = nvmlDeviceGetCount()
        for i in range(device_count):
            handle = nvmlDeviceGetHandleByIndex(i)
            mem_info = nvmlDeviceGetMemoryInfo(handle)
            gpu_info['{}'.format(i)] = '{}MiB'.format(int(mem_info.free/(1024*1024))) + '/' + \
                                       '{}MiB'.format(int(mem_info.total/(1024*1024)))
        print gpu_info
        return gpu_info

    def get_free_gpu(self):
        free_gpu = {}
        free_gpu_id = []
        device_count = nvmlDeviceGetCount()
        for i in range(device_count):
            handle = nvmlDeviceGetHandleByIndex(i)
            mem_info = nvmlDeviceGetMemoryInfo(handle)
            info = nvmlDeviceGetComputeRunningProcesses(handle)
            if len(info) == 0:
                free_gpu['{}'.format(i)] = '{}MiB'.format(int(mem_info.free/(1024*1024)))
                free_gpu_id.append(i)
        print free_gpu
        return free_gpu_id


if __name__ == '__main__':
    gpu = GpuManager()
    a = gpu.get_free_gpu()
    print a
    c = gpu.get_gpu_memory_infos()

