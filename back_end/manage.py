#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys
import ctypes
import platform
import win32file


from back_end.util.setFilePath import SaveDicomFilePath, SaveVolumeFilePath


if __name__ == "__main__":
    so = ctypes.cdll.LoadLibrary
    if platform.system == 'linux':
        lib = so("../netbase/libMcsfLogger.so")
    else:
        lib = so("../netbase/McsfLogger.dll")
    lib.GLogLoadConfig("../netbase/log_config.xml")

    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "back_end.settings")
    try:
        from django.core.management import execute_from_command_line
    except ImportError:
        # The above import may fail for some other reason. Ensure that the
        # issue is really that Django is missing to avoid masking other
        # exceptions on Python 2.
        try:
            import django
        except ImportError:
            raise ImportError(
                "Couldn't import Django. Are you sure it's installed and "
                "available on your PYTHONPATH environment variable? Did you "
                "forget to activate a virtual environment?"
            )
        raise

    if os.path.exists(SaveDicomFilePath.location_1):
        pass
    else:
        os.mkdir(SaveDicomFilePath.location_1)

    if os.path.exists(SaveDicomFilePath.location_2):
        pass
    else:
        os.mkdir(SaveDicomFilePath.location_2)

    if os.path.exists(SaveDicomFilePath.location_3):
        pass
    else:
        os.mkdir(SaveDicomFilePath.location_3)

    if os.path.exists(SaveVolumeFilePath.volumepath):
        pass
    else:
        os.mkdir(SaveVolumeFilePath.volumepath)

    # Start a new thread for reactor loop
    # try:
    #     reactor.connectTCP('127.0.0.1', 8001, be_factory)
    #     thread.start_new_thread(reactor.run, (0,))
    # except Exception as e:
    #     print('初始化连接image_server失败')

    if os.name == 'nt':
        win32file._setmaxstdio(2048)

    execute_from_command_line(sys.argv)
