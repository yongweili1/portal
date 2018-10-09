import struct
import sys

from twisted.internet import reactor
from twisted.internet.protocol import Protocol, Factory, ClientCreator

from message import RequestData
from utilities import get_args


class Transfer(Protocol):
    def __init__(self):
        self.package_size = 0
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

    def dataReceived(self, data):
        # get header info, include total package size
        if self.package_size == 0:
            self.package_size, data = self.unpacking(data)

        self.received_data += data
        self.received_size = len(self.received_data)

        if self.received_size == self.package_size:
            print('[Dispatcher]Total received: {} bytes'.format(self.received_size))

            data = RequestData(self.received_data)

            if data.session not in self.session_mgr:
                self.session_mgr[data.session] = self.protocols.pop(0)
            p = self.session_mgr[data.session]
            p.transport.write(self.package(self.received_data))

            self.package_size = 0
            self.received_size = 0
            self.received_data = ''

    def connectionLost(self, reason):
        self.transport.loseConnection()

    def unpacking(self, data):
        size, = struct.unpack('i', data[:4])
        print('[Dispatcher]package size {} bytes'.format(size))
        data = data[4:]
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
    reactor.run()
