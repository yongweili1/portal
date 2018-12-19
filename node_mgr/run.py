import os
import subprocess

if __name__ == '__main__':
    node_root = os.path.dirname(os.path.abspath(os.path.realpath(__file__)))
    center_node_path = os.path.join(node_root, 'center_node')
    worker_node_path = os.path.join(node_root, 'worker_node')
    pro_list = [subprocess.Popen('python center_node.py', shell=False, cwd=center_node_path),
                subprocess.Popen('python worker_node.py', shell=False, cwd=worker_node_path)]
    for p in pro_list:
        p.wait()
