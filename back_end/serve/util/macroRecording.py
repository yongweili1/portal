# -*-coding:utf-8-*-
import functools
import os
import sys

reload(sys)
sys.setdefaultencoding('gbk')


class Macro(object):
    macro_status = False
    code = ''
    code_header = """# -*- coding: utf-8 -*-\r\n
    import os, django\r\n
    import sys\r\n
    import thread\r\n
    from twisted_client import be_factory\r\n
    from twisted.internet import reactor\r\n
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "back_end.settings")\r\n
    django.setup()\r\n
    sys.path.insert(0, r'E:\\research_ly\portal\\back_end\\apps')\r\n
    seriesuid = sys.argv[1]
    volumepath = sys.argv[2]\r\n
    """.replace('    ', '')

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
                self.__write_line('from apps.connect_image.view_model import %s' % func.__name__)

                data_str = ''
                data_str += '('
                for key, value in kwargs.items():
                    if key == 'seriesuid':
                        data_str += "{0}={0}, ".format(key)
                        continue
                    if key == 'volumepath':
                        data_str += "{0}=r'{1}'.format(volumepath),".format(key, '{}')
                        continue
                    data_str += "{0}='{1}',".format(key, value)
                data_str += ')'

                func_name = func.__name__

                self.__write_line('result = {}'.format(func_name) + data_str + '\n' + 'print(result.kwargs)')
            else:
                pass

            return func(*args, **kwargs)

        return wrapper


# reactor.connectTCP('127.0.0.1', 8001, be_factory)\r\n
# thread.start_new_thread(reactor.run, (0,))\r\n
