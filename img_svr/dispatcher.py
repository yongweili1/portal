import struct
import sys

from twisted.internet import reactor
from twisted.internet.protocol import Protocol, Factory, ClientCreator

from message import RequestData
from utilities import get_args


class Transfer(Protocol):
    def __init__(self):
        self.current_package_size = 0
        self.current_package_data = ''

        self.received_size = 0
        self.received_data = ''

        self.session_mgr = {}
        self.protocols = []
        self.ports = [8002, 8003]

    def connectionMade(self):
        while len(self.ports) > 0:
            port = self.ports.pop(0)
            c = ClientCreator(reactor, ClientTransfer)
            c.connectTCP("127.0.0.1", port).addCallback(self.set_protocol, port=port)

    def set_protocol(self, p, port):
        p.set_protocol(self)
        self.protocols.append(p)
        print('create a new protocol on port {}'.format(port))

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
                self.__send_data_to_server()
                self.current_package_data = ''
                self.current_package_size = 0
            else:
                self.current_package_size -= self.received_size
                self.current_package_data += self.__pop(self.received_size)

    def __send_data_to_server(self):
        print('[Dispatcher]Send to server package size: {} bytes'.format(len(self.current_package_data)))
        data = RequestData(self.current_package_data)
        if data.session not in self.session_mgr:
            self.session_mgr[data.session] = self.protocols.pop(0)
        p = self.session_mgr[data.session]
        p.transport.write(self.package(self.current_package_data))

    def connectionLost(self, reason):
        self.transport.loseConnection()

    def unpacking(self, data):
        size, = struct.unpack('i', data[:4])
        print('[Dispatcher]package size {} bytes'.format(size))
        data = data[4:size + 4]
        return size, data

    def package(self, data):
        size = len(data)
        return struct.pack('i', size) + data


class ClientTransfer(Protocol):
    def __init__(self):
        pass

    def set_protocol(self, p):
        self.server = p
        self.server.transport.resumeProducing()
        pass

    def dataReceived(self, data):
        self.server.transport.write(data)
        pass


if __name__ == '__main__':
    kwargs = get_args(sys.argv[1:])
    host = kwargs['host'] if kwargs['host'] is not None else '0.0.0.0'
    port = kwargs['port'] if kwargs['port'] is not None else 0
    factory = Factory()
    factory.protocol = Transfer
    reactor.listenTCP(port, factory)
    print("Dispatcher started, waiting for connection on port {}".format(port))
    reactor.run()
