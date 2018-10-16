#-*-coding:utf-8-*-

import json
import struct

import image_msg_pb2 as msg
from connect_image.macro_recording import Macro
from twisted_client import be_factory


@Macro()
def load_volume(*args, **kwargs):
    f = open(kwargs['volumepath'], 'rb')
    vol = f.read()
    f.close()

    data = msg.RequestMsg()
    data.session = kwargs['user_ip']
    data.server_name = kwargs['server_name']
    data.command = kwargs['server_name']
    data.content.params = json.dumps({'seriesuid': kwargs['serid']})
    data.content.volume = vol
    data = data.SerializeToString()
    size = len(data)
    size = struct.pack('i', size)
    data = size + data

    be_factory.protocol.request(data)
    rst = be_factory.protocol.waiting_for_result()
    return rst


@Macro()
def get_image(*args, **kwargs):
    data = msg.RequestMsg()
    data.session = kwargs['user_ip']
    data.server_name = kwargs['server_name']
    data.command = kwargs['server_name']
    data.content.params = json.dumps(kwargs)
    data = data.SerializeToString()
    size = len(data)
    size = struct.pack('i', size)
    data = size + data

    be_factory.protocol.request(data)
    rst = be_factory.protocol.waiting_for_result()
    return rst
