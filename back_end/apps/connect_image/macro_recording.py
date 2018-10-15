# -*-coding:utf-8-*-
import functools
import json
import os
import struct
import time
import sys

reload(sys)
sys.setdefaultencoding('gbk')

import image_msg_pb2 as msg


class Macro(object):
    macro_status = False
    code = ''
    code_header = """# -*- coding: utf-8 -*-\r\n
                     import os, django\r\n
                     os.environ.setdefault("DJANGO_SETTINGS_MODULE", "back_end.settings")\r\n 
                     django.setup()\r\n
                     import sys\r\n
                     sys.path.insert(0, r'E:\\research_ly\portal\\back_end\\apps\connect_image')\r\n
                  """

    def __init__(self):
        pass

    def __write_line(self, line, level=0):
        Macro.code += '    ' * level + line + '\r\n'

    def __call__(self, func):
        self.func = func

        @functools.wraps(func)
        def wrapper(*args, **kwargs):

            if self.macro_status:
                file_path = sys._getframe().f_code.co_filename
                file_name = os.path.splitext(os.path.basename(file_path))[0]
                self.__write_line('from view_model import %s' % func.__name__)

                data_str = ''
                data_str += '('
                for key, value in kwargs.items():
                    if key == 'volumepath':
                        data_str += "{0}=r'{1}',".format(key, value)
                        continue
                    data_str += "{0}='{1}',".format(key, value)
                data_str += ')'

                func_name = func.__name__

                self.__write_line('result = {}'.format(func_name) + data_str)
            else:
                pass

            return func(*args, **kwargs)

        return wrapper

