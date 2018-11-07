import os
import platform
import ctypes
import sys
import threading
import time


class PyLogInstance(object):
    _instance_lock = threading.Lock()

    def __init__(self):
        self._source_name = 'default_module'
        path = os.path.dirname(os.path.abspath(__file__))
        if platform.system == 'linux':
            path = os.path.join(path, "linux/libMcsfLogger.so")
        else:
            path = os.path.join(path, "McsfLogger.dll")

        try:
            self._lib = ctypes.cdll.LoadLibrary(path)
        except Exception as e:
            print e

    def __new__(cls, *args, **kwargs):
        if not hasattr(PyLogInstance, "_instance"):
            with PyLogInstance._instance_lock:
                if not hasattr(PyLogInstance, "_instance"):
                    PyLogInstance._instance = object.__new__(cls)
        return PyLogInstance._instance

    def create_log(self, xml=''):
        if len(xml) == 0:
            xml = 'log_config.xml'
        self._lib.GLogLoadConfig(xml)

    def erase_log(self):
        self._lib.GLogUnloadConfig()

    def dev_info(self, description, source='', uid=0):
        self._build_log('LOG_DEV_INFO', '', uid, description, source)

    def dev_warning(self, description, source='', uid=0):
        self._build_log('LOG_DEV_WARNING', '', uid, description, source)

    def dev_error(self, description, source='', uid=0):
        self._build_log('LOG_DEV_ERROR', '', uid, description, source)

    def _build_log(self, log_type, code_info, log_uid, desc, source):
        if '' == source:
            source = self._source_name
        log = [log_type, '\x0002', long(time.time() * 1000), '\x0002', source,
               "({0}:{1})".format(os.getpid(), threading.currentThread().ident)]
        if code_info == '':
            log.append(os.path.basename(sys._getframe().f_code.co_filename))
            log.append('\x0002')
            log.append(sys._getframe().f_code.co_name)
            log.append('\x0002')
            log.append(sys._getframe().f_lineno)
            log.append('\x0002')

        log.append(hex(log_uid))
        log.append("\x0002")

        if len(desc) > 512:
            log.append(desc[:512])
        else:
            log.append(desc)
        log.append('\x0001\n')
        log_content = ''.join(list(map(bytes, log)))
        print log_content
        self._lib.GLogWriteToBuffer(log_content)


if __name__ == '__main__':
    log_inst = PyLogInstance()
    log_inst.create_log()
    for i in range(100):
        log_inst.dev_info('give me a number {0}'.format(i))
        time.sleep(1)

    time.sleep(1000)
