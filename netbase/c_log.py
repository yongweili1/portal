import os
import ctypes
import platform
import sys
import threading
import time
import xml.etree.ElementTree as XmlEt


class PyLogInstance(object):
    _instance_lock = threading.Lock()

    def __init__(self):
        self._source_name = ''
        self._frame_switch = 0
        self._address = ''

        try:
            bin_dir = os.path.dirname(os.path.abspath(os.path.realpath(__file__)))
            if platform.system() == 'Linux':
                lib_name = os.path.join(bin_dir, 'libMcsfLogger.so')
            else:
                lib_name = os.path.join(bin_dir, 'McsfLogger.dll')

            local_log_path = os.path.join(bin_dir, 'log')
            if not os.path.isdir(local_log_path):
                os.makedirs(local_log_path)
            self._lib = ctypes.cdll.LoadLibrary(lib_name)
            self._lib.GSetLocalLogPath(local_log_path)
        except Exception as e:
            print e

    def __new__(cls, *args, **kwargs):
        if not hasattr(PyLogInstance, "_instance"):
            with PyLogInstance._instance_lock:
                if not hasattr(PyLogInstance, "_instance"):
                    PyLogInstance._instance = object.__new__(cls)
        return PyLogInstance._instance

    def create_log(self, server_addr='', xml_path=''):
        if '' == xml_path:
            path = os.path.dirname(os.path.abspath(__file__))
            xml_path = os.path.join(path, 'config/log_config.xml')

        tree = XmlEt.parse(xml_path)
        root = tree.getroot()
        client_name = root.find('LOG_CLIENT_NAME').text
        if '' != client_name:
            self._source_name = client_name
        if root.find('LOG_TRACE_SOURCE_CODE_INFO').text.lower() == 'on':
            self._frame_switch = 1

        if '' != server_addr:
            ip, port = server_addr.split(':', 1)
            root.find('ServerIP').text = ip
            root.find('LogServerPort').text = port
            tree.write(xml_path)
        self._address = '{}:{}'.format(root.find('ServerIP').text, root.find('LogServerPort').text)

        self._lib.GLogLoadConfig(xml_path)

    def erase_log(self):
        self._lib.GLogUnloadConfig()

    def dev_info(self, description, source='', uid=0):
        self._build_log('LOG_DEV_INFO', '', uid, description, source)

    def set_source(self, source):
        if '' != source:
            self._source_name = source

    def dev_warning(self, description, source='', uid=0):
        self._build_log('LOG_DEV_WARNING', '', uid, description, source)

    def dev_error(self, description, source='', uid=0):
        self._build_log('LOG_DEV_ERROR', '', uid, description, source)

    def _build_log(self, log_type, code_info, log_uid, desc, source):
        if '' == source:
            source = self._source_name
        log = [log_type, '\x02', long(time.time() * 1000), '\x02', source,
               "({0}:{1})".format(os.getpid(), threading.currentThread().ident), '\x02']
        if code_info == '':
            log.append(os.path.basename(sys._getframe(2).f_code.co_filename) if self._frame_switch else 'unknown')
            log.append('\x02')
            log.append(sys._getframe(2).f_lineno if self._frame_switch else 'unknown')
            log.append('\x02')
            log.append(sys._getframe(2).f_code.co_name if self._frame_switch else 'unknown')
            log.append('\x02')

        log.append(hex(log_uid))
        log.append("\x02")
        desc = str(desc)
        log.append(desc[:512] if len(desc) > 512 else desc)

        log.append('\x01\n')
        log_content = ''.join(list(map(bytes, log)))
        self._lib.GLogWriteToBuffer(log_content)

    def get_address(self):
        return self._address


log = PyLogInstance()


def test():
    log.create_log()
    for i in range(100):
        log.dev_info('give me a number {0}'.format(i))
        time.sleep(1)


if __name__ == '__main__':
    test()

