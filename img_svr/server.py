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
        self.current_package_size = 0
        self.current_package_data = ''

        self.received_size = 0
        self.received_data = ''

    def connectionMade(self):
        log.msg("New connection, the info is:", self.transport.getPeer())

    def connectionLost(self, reason):
        print('Connection lost, reason: ', reason)

    def __pop(self, length=0, type='data'):
        data = self.received_data[:length]
        if type == 'head':
            data, = struct.unpack('i', data)
        self.received_data = self.received_data[length:]
        self.received_size = len(self.received_data)
        return data

    def dataReceived(self, data):
        self.received_data = data
        self.received_size = len(data)

        while self.received_data != '':
            if self.current_package_size == 0:
                self.current_package_size = self.__pop(4, type='head')
                print('[Dispatcher]Need to received package size: {} bytes'.format(self.current_package_size))

            if self.current_package_size <= self.received_size:
                self.current_package_data += self.__pop(self.current_package_size)
                self.__handle()
                self.current_package_data = ''
                self.current_package_size = 0
            else:
                self.current_package_size -= self.received_size
                self.current_package_data += self.__pop(self.received_size)

    def __handle(self):
        print('[Server]Total data size: {} bytes'.format(len(self.current_package_data)))
        data = RequestData(self.current_package_data)
        response_data = cmd.commands[data.command](**data.kwargs)
        self.transport.write(self.package(response_data))

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

