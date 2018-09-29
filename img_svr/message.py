import json


class RequestMsg(object):
    def __init__(self, data):
        kwargs = eval(data)
        self.session = kwargs['session']
        self.server = kwargs['server_name']
        self.command = kwargs['command']
        self.content = kwargs['content']

    def content(self, key=None):
        if key is None:
            return self.content
        if key in self.content:
            return self.content[key]
        else:
            return None


class ResponseMsg(object):
    def __init__(self, content, success=True, msg=''):
        self.success = success
        self.msg = msg
        self.content = content

    def package(self):
        dic = {'success': self.success,
               'msg': self.msg,
               'context': self.content }
        return json.dumps(dic)
