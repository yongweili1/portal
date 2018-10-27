#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import platform
import sys
import thread
import win32file

from twisted.internet import reactor

from back_end.util.setFilePath import SaveDicomFilePath, SaveVolumeFilePath, LinuxFilePath
from twisted_client import be_factory

if __name__ == "__main__":
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

    if platform.system() == 'Windows':
        win32file._setmaxstdio(2048)

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
    elif platform.system() == 'Linux':
        if os.path.exists(LinuxFilePath.dicomPath):
            pass
        else:
            os.mkdir(LinuxFilePath.dicomPath)

        if os.path.exists(LinuxFilePath.splitDicomPath):
            pass
        else:
            os.mkdir(LinuxFilePath.splitDicomPath)

        if os.path.exists(LinuxFilePath.volumePath):
            pass
        else:
            os.mkdir(LinuxFilePath.volumePath)
    else:
        pass

    execute_from_command_line(sys.argv)
