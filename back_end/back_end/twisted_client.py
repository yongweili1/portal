from __future__ import print_function

from twisted.internet import reactor, protocol

import struct
from apps.connect_image import image_msg_pb2 as msg


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
        self.current_package_size = 0
        self.current_package_data = ''

        self.received_size = 0
        self.received_data = ''

        self.DATA = None
        print('BackEndClient init')

    def connectionMade(self):
        print("New connection", self.transport.getPeer())

    def connectionLost(self, reason):
        print("connection lost, reason:", reason)

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
                self.__handle_data_from_server()
                self.current_package_data = ''
                self.current_package_size = 0
            else:
                self.current_package_size -= self.received_size
                self.current_package_data += self.__pop(self.received_size)

    def waiting_for_result(self, timeout=60):
        while 1:
            if self.DATA is not None:
                rst = self.DATA
                self.DATA = None
                return rst

    def __handle_data_from_server(self):
        print('[BE]Total data size: {} bytes'.format(len(self.current_package_data)))
        self.DATA = ResponseData(self.current_package_data)

    def send_data_to_server(self, send_content):
        print(len(send_content))
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

