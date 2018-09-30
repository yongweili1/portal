import json

from command import command as cmd
from message import RequestData
from utilities import get_args

from twisted.internet.protocol import Factory, Protocol
from twisted.internet import reactor
import struct
from twisted.python import log
import sys
log.startLogging(sys.stdout)


class Server(Protocol):
    def __init__(self):
        self.package_size = 0
        self.received_size = 0
        self.received_data = ''

    def connectionMade(self):
        log.msg("New connection, the info is:", self.transport.getPeer())

    def connectionLost(self, reason):
        print('Connection lost, reason: ', reason)

    def dataReceived(self, data):
        # get header info, include total package size
        if self.package_size == 0:
            self.package_size, data = self.unpacking(data)

        self.received_data += data
        self.received_size = len(self.received_data)
        print('Total received: {} bytes'.format(self.received_size))

        if self.received_size == self.package_size:
            print('Received all data')

            data = RequestData(self.received_data)
            self.package_size = 0
            self.received_size = 0
            self.received_data = ''

            response_data = cmd.commands[data.command](**data.kwargs)
            self.back_to_client(response_data)

    def back_to_client(self, response_data):
        send_content = self.package(response_data)
        self.transport.write(send_content)

    def unpacking(self, data):
        size, = struct.unpack('i', data[:4])
        print('package size {} bytes'.format(size))
        data = data[4:]
        return size, data

    def package(self, data):
        size = len(data)
        return struct.pack('i', size) + data


class ServerFactory(Factory):
    def __init__(self):
        pass

    def buildProtocol(self, addr):
        return Server()


if __name__ == '__main__':
    kwargs = get_args(sys.argv[1:])
    host = kwargs['host'] if kwargs['host'] is not None else '0.0.0.0'
    port = kwargs['port'] if kwargs['port'] is not None else 0
    reactor.listenTCP(port, ServerFactory())
    reactor.run()

