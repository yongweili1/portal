import subprocess

class ProcessManager(object):
    def __init__(self):
        self.pro_list = {}

    def create_render_srv(self, proxy_name, command_line):
        child = subprocess.Popen(command_line)
        self.pro_list[]

if __name__ == '__main__':
