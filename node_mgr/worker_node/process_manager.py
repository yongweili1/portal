import subprocess
import os
import subprocess
import time

import ctypes
import sys



class ProcessManager(object):
    def __init__(self):
        self.pro_list = {}
        self.pro_name_index = 0


    def create_render_srv(self, proxy_name, command_line):
        child = subprocess.Popen(command_line)


if __name__ == '__main__':

    def start_batch_render(self, number, node_name):
        for x in range(number):
            self.pro_name_index += 1
            proxy_name = "{}#img_srv#{}".format(node_name, self.pro_name_index)
            cmd_line = ['calc.exe']
            self.create_render_srv(proxy_name, cmd_line)
        pass

    def create_render_srv(self, proxy_name, command_line):
        child = subprocess.Popen(command_line, shell=False)
        print '---{}---'.format(child.pid)
        self.pro_list[proxy_name] = child

    def kill_render_srv(self, proxy_name):
        if proxy_name in self.pro_list:
            self.pro_list[proxy_name].terminate()
            self.pro_list.pop(proxy_name)

    def kill_all(self):
        for prc in self.pro_list.values():
            prc.terminate()
            os.kill()

        self.pro_list.clear()


if __name__ == '__main__':
    proc_mgr = ProcessManager()
    proc_mgr.start_batch_render(4, '11')
    time.sleep(1000)
    print len(proc_mgr.pro_list)
    proc_mgr.kill_all()
    print len(proc_mgr.pro_list)
