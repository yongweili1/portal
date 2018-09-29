import getopt
import socket
import sys
import json


class SocketServer(object):
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.sock = None
        self.__create_sock(self.host, self.port)

    def __create_sock(self, host, port):
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except socket.error:
            print "Failed to create socket"
            sys.exit()
        print 'Socket Created, port {}'.format(port)

        try:
            remote_ip = socket.gethostbyname(host)
        except socket.gaierror:
            print "Hostname could not be resolved. Exiting"
            sys.exit()
        self.sock.connect((remote_ip, port))
        print "Socket Connect to %s on ip %s" % (host, remote_ip)

    def request(self, **kwargs):
        try:
            data = json.dumps(kwargs)
            self.sock.sendall(data)
        except socket.error:
            print 'Send failed'
            sys.exit()
        print "Message send successfully"
        # self.sock.setsockopt()
        reply = self.sock.recv(4096000)
        self.sock.close()
        return reply


class Dispatcher(object):
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.conn = None
        self.servers = {}
        self.allowed_port = [8883, 8884, 8885]
        print 'Socket created'

        try:
            self.sock.bind((self.host, self.port))
        except socket.error, msg:
            print 'Bind failed. Error code: %s, Message: %s' % (
                msg[0], msg[1])
            sys.exit()
        print 'Socket bind complete'

        self.sock.listen(10)
        print 'Socket now listening on port {}'.format(self.port)

    def start(self):
        # now keep talking with the client
        while 1:
            self.conn, addr = self.sock.accept()
            print 'Connected with %s:%s' % (addr[0], str(addr[1]))
            data = self.conn.recv(1024)
            if not data:
                break
            data = json.loads(data, encoding='utf-8')
            self.conn.sendall(self.handle_request(**data))

    def close(self):
        self.conn.close()
        self.sock.close()

    def handle_request(self, **kwargs):
        session_id = kwargs['session']
        print(session_id)
        if session_id not in self.servers:
            p = self.allowed_port.pop(0)
            self.servers[session_id] = p

        sock = SocketServer('10.9.19.148', self.servers[session_id])
        print('Create a new socket on port {}'.format(self.servers[session_id]))

        rst = sock.request(**kwargs)
        return rst


if __name__ == '__main__':
    argv = sys.argv[1:]
    host = '10.9.19.148'
    port = 0
    try:
        opts, args = getopt.getopt(argv, "h:p:", ["host=", "port="])
    except getopt.GetoptError:
        print 'server.py -h <host> -p <port>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            host = arg
        elif opt == '-p':
            port = int(arg)

    dispatcher = Dispatcher(host, port)
    dispatcher.start()
