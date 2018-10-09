# -*-coding:utf-8-*-
import functools
import os
import sys
import time


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
        def wrapper(data):

            if self.macro_status:
                file_path = sys._getframe().f_code.co_filename
                file_name = os.path.splitext(os.path.basename(file_path))[0]
                data1 = 'from views import %s\n' % (func.__name__)

                with open('apps/connect_image/macro_test.py', 'a+') as f:
                    func_name = func.__name__
                    data1 = data1 + '\nresult = ' + func_name + '(data)' + '\n'
                    # print(data1)
                    f.write(data1)
            else:
                pass

            return func(data)

        return wrapper

