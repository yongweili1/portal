#-*-coding:utf-8-*-

import json
import time

import image_msg_pb2 as msg
from macro_recording import Macro
import sys
from twisted_client import ResponseData
sys.path.append('..')
from netbase import comproxy
# from twisted_client import be_factory

proxy = comproxy.PyCommProxy("web_be", "127.0.0.1:10000")

@Macro()
def load_volume(*args, **kwargs):
    try:
        f = open(kwargs['volumepath'], 'rb')
        vol = f.read()
        f.close()
    except Exception as e:
        print('请检查volume路径是否正确，{}'.format(kwargs['volumepath']))
        raise

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
