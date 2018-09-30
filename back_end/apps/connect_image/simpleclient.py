
# Copyright (c) Twisted Matrix Laboratories.
# See LICENSE for details.


"""
An example client. Run simpleserv.py first before running this.
"""
from __future__ import print_function

import json

from twisted.internet import reactor, protocol

import struct
import sys
import image_msg_pb2 as msg


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


# a client protocol
class BackEndClient(protocol.Protocol):
    def __init__(self):
        self.package_size = 0
        self.received_size = 0
        self.received_data = ''
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
        print('Total received: {} bytes'.format(self.received_size))

        if self.received_size == self.package_size:
            print('Received all data')

            data = ResponseData(self.received_data)
            self.package_size = 0
            self.received_size = 0
            self.received_data = ''

    def handle_data_from_server(self, pack_data):
        # self.command_func_dict[command_id](pack_data)
        data = msg.ResponseMsg()
        data.ParseFromString(pack_data)
        pass

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

    def send_vol(self):
        volumepath = r'E:\data\SHEN YU-Thorax^10_ZRY_LDCT_Head_first (Adult)-CT.nii.gz'
        f = open(volumepath, 'rb')
        vol = f.read()
        f.close()

        data = msg.RequestMsg()
        data.session = '10.9.19.139'
        data.server_name = 'image'
        data.command = 'load'
        data.content.volume = vol
        data.content.params = json.dumps({'seriesuid': '1111111'})
        data = data.SerializeToString()
        size = len(data)
        size = struct.pack('i', size)
        data = size + data
        self.send_data_to_server(data)

    def show(self):
        # seriesuid=1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373&width=400&height=400&focus_view=&display_view=all
        data = msg.RequestMsg()
        data.session = '10.9.19.139'
        data.server_name = 'image'
        data.command = 'show'
        data.content.params = json.dumps({'seriesuid': '1111111',
                                          'width':400,
                                          'height':400,
                                          'focus_view':'',
                                          'display_view':'all'})
        data = data.SerializeToString()
        size = len(data)
        size = struct.pack('i', size)
        data = size + data
        self.send_data_to_server(data)


class EchoFactory(protocol.ClientFactory):
    def __init__(self):
        print('init')
        self.protocol = BackEndClient()

    def startedConnecting(self, connector):
        print("Started to connect")

    def buildProtocol(self, addr):
        self.protocol.factory = self
        return self.protocol

        print("Connected.")
        return self.protocol

    def clientConnectionFailed(self, connector, reason):
        print("Lost connection. Reason:", reason)
        reactor.stop()

    def clientConnectionLost(self, connector, reason):
        print("Connection failed. Reason:", reason)
        reactor.stop()


# this connects the protocol to a server running on port 8000
def main():
    f = EchoFactory()
    reactor.callLater(3, f.protocol.send_vol)
    reactor.callLater(23, f.protocol.show)
    reactor.connectTCP("127.0.0.1", 8883, f)
    reactor.run()

main()
