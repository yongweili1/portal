# -*-coding:utf-8-*-

import json
import time

from serve.util import imageMsgPb2 as msg
from serve.util.macroRecording import Macro
import sys
sys.path.append('..')
from netbase import comproxy

proxy = comproxy.PyCommProxy("web_be", "127.0.0.1:10000")


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


@Macro()
def load_volume(*args, **kwargs):
    try:
        f = open(kwargs['volumepath'], 'rb')
        vol = f.read()
        f.close()
    except Exception as e:
        print('请检查volume路径或文件格式是否正确:{}'.format(kwargs['volumepath']))
        raise IOError

    data = msg.RequestMsg()
    data.session = kwargs['user_ip']
    data.server_name = kwargs['server_name']
    data.command = kwargs['command']
    data.content.params = json.dumps({'seriesuid': kwargs['seriesuid']})
    data.content.volume = vol
    data = data.SerializeToString()
    rst = proxy.sync_send_command(data, 100, 'img_srv')
    rst = ResponseData(rst)

    return rst


@Macro()
def get_image(*args, **kwargs):
    data = msg.RequestMsg()
    data.session = kwargs['user_ip']
    data.server_name = kwargs['server_name']
    data.command = kwargs['command']
    data.content.params = json.dumps(kwargs)
    data = data.SerializeToString()
    b = time.time()

    rst = proxy.sync_send_command(data, 100, 'img_srv')
    c = time.time()
    rst = ResponseData(rst)

    print('req&resp use {} ms'.format(str((c - b) * 1000)))
    return rst
