
# Copyright (c) Twisted Matrix Laboratories.
# See LICENSE for details.
import sys

from twisted.internet import reactor, protocol

from utilities import get_args


class Echo(protocol.Protocol):
    """This is just about the simplest possible protocol"""

    def __init__(self):
        self.package_size = 0
        self.received_size = 0
        self.data = ''

    def dataReceived(self, data):
        self.received_size += len(data)
        print('received:', self.received_size)

        if self.package_size == 0:
            pass

        if self.received_size == self.package_size:
            self.package_size = 0
            self.received_size = 0
            self.data = ''
            pass
        else:
            self.data += data

    def connectionLost(self, reason):
        print('Connection lost')


def main(port):
    """This runs the protocol on port 8000"""
    factory = protocol.ServerFactory()
    factory.protocol = Echo
    reactor.listenTCP(port, factory)
    reactor.run()


# this only runs if the module was *not* imported
if __name__ == '__main__':
    kwargs = get_args(sys.argv[1:])
    host = kwargs['host'] if kwargs['host'] is not None else '0.0.0.0'
    port = kwargs['port'] if kwargs['port'] is not None else 0
    main(port)
