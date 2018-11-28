import json
import os

from msg import image_msg_pb2 as msg


class RequestData(object):
    def __init__(self, request_data):
        self.command = ''
        self.content = {}
        root = os.path.dirname(os.path.abspath(__file__)).split('portal')[0]
        root = os.path.join(root, 'portal_ferry')
        self.dir_path = os.path.join(root, 'srv/volume')
        if not os.path.isdir(self.dir_path):
            os.makedirs(self.dir_path)

        data = msg.RequestMsg()
        data.ParseFromString(request_data)
        self.command = data.command

        if data.content:
            self.content = json.loads(data.content)

    def arg(self, key=None):
        if key is None:
            return self.content
        if key in self.content:
            return self.content[key]
        else:
            return None


class ResponseData(object):
    def __call__(self, content='', success=True, message=''):
        data = msg.ResponseMsg()
        data.success = success
        data.comment = message
        data.content = content
        data = data.SerializeToString()
        return data


# single instance of ResponseData
response = ResponseData()
