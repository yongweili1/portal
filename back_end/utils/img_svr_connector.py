# -*-coding:utf-8-*-

import json

from utils.response_dto import ResponseDto
from be_proxy import proxy
from netbase.cmd_event_id import CmdId
from utils import image_msg_pb2 as msg
from utils.macro_recorder import MacroRecorder


@MacroRecorder()
def sync_send_command(command, **kwargs):
    data = msg.RequestMsg()
    data.command = command
    data.content = json.dumps(kwargs)
    data = data.SerializeToString()
    rst = proxy.sync_send_command(data, CmdId.cmd_id_render, 'img_srv')

    data = msg.ResponseMsg()
    data.ParseFromString(rst)
    return ResponseDto(success=data.success, message=data.comment, data=data.content)


@MacroRecorder()
def sync_load_volume(vol):
    rst = proxy.sync_send_command(vol, CmdId.cmd_id_load_volume, 'img_srv')
    data = msg.ResponseMsg()
    data.ParseFromString(rst)
    return ResponseDto(success=data.success, message=data.comment, data=data.content)
