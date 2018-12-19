#-*-coding:utf-8-*-

from pynvml import *


class GpuManager(object):
    def __init__(self):
        nvmlInit()
        self.device_count = nvmlDeviceGetCount()

    # def get_gpu_memory_infos(self):
    #     gpu_info = {}
    #     device_count = nvmlDeviceGetCount()
    #     for i in range(device_count):
    #         handle = nvmlDeviceGetHandleByIndex(i)
    #         mem_info = nvmlDeviceGetMemoryInfo(handle)
    #         gpu_info['{}'.format(i)] = '{}MiB'.format(int(mem_info.free/(1024*1024))) + '/' + \
    #                                    '{}MiB'.format(int(mem_info.total/(1024*1024)))
    #     print gpu_info
    #     return gpu_info
    #
    def get_free_gpu_list(self):
        free_gpu = []
        for i in range(self.device_count):
            handle = nvmlDeviceGetHandleByIndex(i)
            info = nvmlDeviceGetComputeRunningProcesses(handle)
            if len(info) == 0:
                free_gpu.append(i)


#gpu_mgr = GpuManager()

if __name__ == '__main__':
#    print gpu_mgr.device_count

