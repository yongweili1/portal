import json
from msg import image_msg_pb2 as msg


class RequestData(object):
    def __init__(self, request_data):
        self.session = ''
        self.server_name = ''
        self.command = ''
        self.kwargs = {}

        data = msg.RequestMsg()
        data.ParseFromString(request_data)
        self.session = data.session
        self.server_name = data.server_name
        self.command = data.command

        if data.content and data.content.params:
            self.kwargs = json.loads(data.content.params)
        if data.content and data.content.volume:
            volume_path = r'D:\volume\{}.nii.gz'.format(self.kwargs['seriesuid'])
            f = open(volume_path, 'wb')
            f.write(data.content.volume)
            f.close()
            self.kwargs['volume_path'] = volume_path

    def arg(self, key=None):
        if key is None:
            return self.kwargs
        if key in self.kwargs:
            return self.kwargs[key]
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

