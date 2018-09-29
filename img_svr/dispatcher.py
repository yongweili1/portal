# -*-coding:utf-8-*-

import sys

from socket_factory import create_socket
from utilities import get_args

from msg import image_msg_pb2 as msg


class Dispatcher(object):
    def __init__(self, **kwargs):
        # key: server_name, value: server port
        self.allowed_ports = [8883, 8884, 8885]
        # key: session, value: server port
        self.connected_ports = {}

        host = kwargs['host'] if kwargs['host'] is not None else '0.0.0.0'
        port = kwargs['port'] if kwargs['port'] is not None else 0
        self.sock = create_socket(host, port)

    def start(self):
        while 1:
            conn, addr = self.sock.accept()
            conn.settimeout(600)
            recv_data = conn.recv(8388608)
            if not recv_data:
                continue

            data = msg.RequestMsg()
            data.ParseFromString(recv_data)
            session = data.session
            operation = data.content.params
            if operation == 'register':
                if session not in self.connected_ports:
                    port = self.allowed_ports.pop(0)
                    self.connected_ports[session] = port
                else:
                    port = self.connected_ports[session]
                print(port)
                conn.sendall(str(port))
            elif operation == 'unregister':
                if session in self.connected_ports:
                    port = self.connected_ports.pop(session)
                    self.allowed_ports.append(port)
                    conn.sendall('0')
            else:
                conn.sendall('0')


if __name__ == '__main__':
    kwargs = get_args(sys.argv[1:])
    dispatcher = Dispatcher(**kwargs)
    dispatcher.start()

