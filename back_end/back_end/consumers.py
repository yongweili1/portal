# -*-coding:utf-8-*-
from channels.handler import AsgiHandler

# message.reply_channel    一个客户端通道的对象
# message.reply_channel.send(chunk)  用来唯一返回这个客户端

# 一个管道大概会持续30s


def ws_connect(message):
    message.reply_channel.send({"text": "connect"})


def ws_message(message):
    mess = Message()
    mess.channels_status = True
    data = {"text": "text"}
    mess.send_message(message, data)


def ws_disconnect(message):
    mess = Message()
    mess.channels_status = False
    data = {"text": "disconnect"}
    mess.send_message(message, data)


class Message(object):

    def __init__(self):
        self.channels_status = False

    def send_message(self, message, data):
        while self.channels_status:
            message.reply_channel.send(data)
