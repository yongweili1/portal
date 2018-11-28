# -*-coding:utf-8-*-

import json

from utils.response_dto import ResponseDto
from be_proxy import proxy
from utils import image_msg_pb2 as msg
from utils.macro_recorder import MacroRecorder


class ResponseData(object):
    def __init__(self, response_data):
        self.success = True
        self.comment = ''
        self.content = ''

        data = msg.ResponseMsg()
        data.ParseFromString(response_data)
        self.success = data.success
        self.comment = data.comment

        if data.content:
            self.content = data.content

    def arg(self, key=None):
        if key is None:
            return self.content
        if key in self.content:
            return self.content[key]
        else:
            return None


@MacroRecorder()
def handle_command(command, **kwargs):
    data = msg.RequestMsg()
    data.command = command
    data.content = json.dumps(kwargs)
    data = data.SerializeToString()
    rst = proxy.sync_send_command(data, 100, 'img_srv')

    data = msg.ResponseMsg()
    data.ParseFromString(rst)
    return ResponseDto(success=data.success, message=data.comment, data=data.content)
