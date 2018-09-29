import socket
import sys, getopt
import json
from api.image_server import ImageServer


class Server(object):
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.conn = None
        self.image_server = ImageServer()
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
            data = self.conn.recv(1024000)
            data = json.loads(data, encoding='utf-8')
            if not data:
                break
            command_id = data['command']
            self.image_server.index = self.image_server.index + 1
            self.conn.sendall(str(self.image_server.index))

    def close(self):
        self.conn.close()
        self.sock.close()


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

    dispatcher = Server(host, port)
    dispatcher.start()
