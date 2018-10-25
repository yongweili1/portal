import os
import platform
import ctypes
import sys
import time

so = ctypes.cdll.LoadLibrary
if platform.system == 'linux':
    lib = so("libMcsfLogger.so")
else:
    lib = so("McsfLogger.dll")


def log_load_config():
    lib.GLogLoadConfig("log_config.xml")


class Threading(object):
    pass


def build_log(log_type, code_info, source, log_uid, desc):
    _log = [log_type, '\x0002', long(time.time() * 1000)]
    if source == '':
        source = 'default_source'
    _log.append(source)
    _log.append("(%d:)" % os.getpid())
    if code_info == '':
        _log.append(os.path.basename(sys._getframe().f_code.co_filename))
        _log.append('\x0002')
        _log.append(sys._getframe().f_code.co_name)
        _log.append('\x0002')
        _log.append(sys._getframe().f_lineno)
        _log.append('\x0002')

    _log.append(hex(log_uid))
    _log.append("\x0002")

    if len(desc) > 512:
        _log.append(desc[:512])
    else:
        _log.append(desc)
    _log.append('\x0001\n')
    return ''.join(list(map(str, _log)))


if __name__ == '__main__':
    print build_log('LOG_DEV_INFO', '', 'aii', 188888888, 'python first log')
