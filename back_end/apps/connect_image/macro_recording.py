# -*-coding:utf-8-*-
import functools
import os
import time
import sys

reload(sys)
sys.setdefaultencoding('gbk')

import image_msg_pb2 as msg

# class Script(object):
#
#     def __init__(self):
#         self.imp = 'from twisted_client import be_factory\n'
#         self.template = 'be_factory.protocol.request({})\nrst = be_factory.protocol.waiting_for_result()\n'
#         self.is_first = True
#
#     def write(self, data):
#         if self.is_first:
#             script = self.imp + self.template
#
#     def close(self, data):
#         pass
#
#
# script = Script()


class Macro(object):

    macro_status = False

    def __init__(self):
        pass

    def __call__(self, func):
        self.func = func

        @functools.wraps(func)
        def wrapper(*args, **kwargs):

            if self.macro_status:
                file_path = sys._getframe().f_code.co_filename
                file_name = os.path.splitext(os.path.basename(file_path))[0]
                data = 'from views import %s\n' % (func.__name__)

                with open('apps/connect_image/macro_test.py', 'a+') as f:
                    func_name = func.__name__
                    if kwargs['data']:
                        request_data = kwargs['data'].ParseFromString()
                        data += 'data = ' + '"""' + str(request_data) + '"""' + '\n'
                    volumepath = kwargs.get('volumepath', None)
                    if volumepath is not None:
                        data += 'volumepath = r' + '"' + str(kwargs['volumepath']) + '"' + '\n'
                    data = data + 'result = ' + func_name + '(data)' + '\n'
                    # print(data1)
                    f.write(data)
            else:
                pass

            return func(*args, **kwargs)

        return wrapper

