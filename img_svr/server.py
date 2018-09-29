import json
import sys
from command import command as cmd
from message import RequestMsg
from socket_factory import create_socket
from utilities import get_args

from msg import image_msg_pb2 as msg


class Server(object):
    def __init__(self, **kwargs):
        host = kwargs['host'] if kwargs['host'] is not None else '0.0.0.0'
        port = kwargs['port'] if kwargs['port'] is not None else 0
        self.sock = create_socket(host, port)

    def get_data(self, size, conn):
        data = ''
        c = 0
        while 1:
            data += conn.recv(8388608)
            c += 1
            print('Count {}, total data size {} bytes'.format(c, sys.getsizeof(data)))
            if size - sys.getsizeof(data) < 10000:
                conn.sendall('handle data finished')
                conn.close()
                break
            else:
                continue
        return data

    def start(self):
        DATA = ''
        count = 0
        while 1:
            conn, addr = self.sock.accept()
            conn.settimeout(600)

            recv_data = conn.recv(8388608)
            if not recv_data:
                continue
            if recv_data == 'start':
                DATA = ''
                count = 0
                print 'Start accept data from {}:{}'.format(addr[0], str(addr[1]))
                conn.sendall('Started.')
            elif recv_data == 'finish':
                print 'Accepted all data.'
                rst = self.handle_request(DATA)
                conn.sendall(rst)
            else:
                DATA += recv_data
                count += 1
                current_data_size = sys.getsizeof(recv_data)
                total_data_size = sys.getsizeof(DATA)
                print('Count {}, current data size {} bytes, total data size {} bytes'
                      .format(count, current_data_size, total_data_size))
                conn.sendall('handle data {}'.format(count))

    def handle_request(self, recv_data):
        data = msg.RequestMsg()
        data.ParseFromString(recv_data)
        params = json.loads(data.content.params)
        volume = data.content.volume

        if params['server_name'] is not 'image':
            print('Wrong Destination.')
            return 'Wrong Destination.'
        # handle command and return result to dispatcher
        rst = cmd.commands[params['command']](volume)
        return rst


if __name__ == '__main__':
    kwargs = get_args(sys.argv[1:])
    dispatcher = Server(**kwargs)
    dispatcher.start()
