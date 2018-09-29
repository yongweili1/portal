# -*-coding:utf-8-*-
import socket
import sys


def create_socket():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('10.9.19.148', 8002))
    print('Connet with server')
    return sock


def create_app_socket(port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('10.9.19.148', port))
    print('Connet with server')
    return sock


class SocketClient(object):

    def upload_volume(self, data):

        sock_client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = ('127.0.0.1', 8889)
        sock_client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock_client.connect(server_address)
        sock_client.sendall(data)

        reply = sock_client.recv(4096000)
        sock_client.close()
        return reply


# if __name__ == '__main__':
#     upload_volume()


# class SocketClient(object):
#     def __init__(self, host, port):
#         self.host = host
#         self.port = port
#         self.sock = None
#         self.__create_sock(self.host, self.port)
#
#     def __create_sock(self, host, port):
#         try:
#             self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#         except socket.error:
#             print "Failed to create socket"
#             sys.exit()
#         print 'Socket Created'
#
#         try:
#             remote_ip = socket.gethostbyname(host)
#         except socket.gaierror:
#             print "Hostname could not be resolved. Exiting"
#             sys.exit()
#         nSize = 1024 * 1024 * 8
#         self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, nSize)
#         self.sock.connect((remote_ip, port))
#         print "Socket Connect to %s on ip %s" % (host, remote_ip)
#
#     def request(self, data):
#         try:
#             print(sys.getsizeof(data))
#             self.sock.sendall(data)
#         except socket.error:
#             print 'Send failed'
#             sys.exit()
#         print "Message send successfully"
#         # self.sock.setsockopt()
#         reply = self.sock.recv(4096000)
#         self.sock.close()
#         return reply
