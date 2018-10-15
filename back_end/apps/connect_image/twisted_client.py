from __future__ import print_function

from twisted.internet import reactor, protocol

import struct
from connect_image import image_msg_pb2 as msg


class ResponseData(object):
    def __init__(self, response_data):
        self.success = True
        self.comment = ''
        self.kwargs = ''

        data = msg.ResponseMsg()
        data.ParseFromString(response_data)
        self.success = data.success
        self.comment = data.comment

        if data.content:
            self.kwargs = data.content

    def arg(self, key=None):
        if key is None:
            return self.kwargs
        if key in self.kwargs:
            return self.kwargs[key]
        else:
            return None


class BackEndClient(protocol.Protocol):
    def __init__(self):
        self.package_size = 0
        self.received_size = 0
        self.received_data = ''
        self.DATA = None
        print('BackEndClient init')

    def connectionMade(self):
        print("New connection", self.transport.getPeer())

    def connectionLost(self, reason):
        print("connection lost, reason:", reason)

    def dataReceived(self, data):
        # get header info, include total package size
        if self.package_size == 0:
            self.package_size, data = self.unpacking(data)

        self.received_data += data
        self.received_size = len(self.received_data)
        # print('Total received: {} bytes'.format(self.received_size))

        if self.received_size == self.package_size:
            # print('Received all data')
            self.handle_data_from_server()

    def waiting_for_result(self, timeout=60):
        while 1:
            if self.DATA is not None:
                rst = self.DATA
                self.DATA = None
                return rst

    def handle_data_from_server(self):
        self.DATA = ResponseData(self.received_data)
        self.package_size = 0
        self.received_size = 0
        self.received_data = ''

    def send_data_to_server(self, send_content):
        self.transport.write(send_content)

    def unpacking(self, data):
        size, = struct.unpack('i', data[:4])
        print('package size {} bytes'.format(size))
        data = data[4:]
        return size, data

    def package(self, data):
        size = len(data)
        return struct.pack('i', size) + data

    def request(self, data):
        self.send_data_to_server(data)


class BackEndFactory(protocol.ClientFactory):
    def __init__(self):
        self.protocol = BackEndClient()

    def startedConnecting(self, connector):
        print("Started to connect")

    def buildProtocol(self, addr):
        self.protocol.factory = self
        print("Connected.")
        return self.protocol

    def clientConnectionFailed(self, connector, reason):
        print("Lost connection. Reason:", reason)
        reactor.stop()

    def clientConnectionLost(self, connector, reason):
        print("Connection failed. Reason:", reason)


# single instance of BackEndFactory
be_factory = BackEndFactory()
