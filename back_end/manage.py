#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import platform
import sys

from serve.static_parameters.setFilePath import SaveDicomFilePath, LinuxFilePath, WindowsFilePath, filepath


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
        import win32file
        win32file._setmaxstdio(2048)
        filepath.set_dcmpath(WindowsFilePath.dicomPath)
        filepath.set_splitdcmpath(WindowsFilePath.splitDicomPath)
        filepath.set_volumepath(WindowsFilePath.volumePath)
        filepath.set_cpspath(WindowsFilePath.cpsPath)
    elif platform.system() == 'Linux':
        filepath.set_dcmpath(LinuxFilePath.dicomPath)
        filepath.set_splitdcmpath(LinuxFilePath.splitDicomPath)
        filepath.set_volumepath(LinuxFilePath.volumePath)
        filepath.set_cpspath(LinuxFilePath.cpsPath)

    else:
        pass

    if os.path.exists(filepath.dicomPath):
        pass
    else:
        os.mkdir(filepath.dicomPath)

    if os.path.exists(filepath.splitDicomPath):
        pass
    else:
        os.mkdir(filepath.splitDicomPath)

    if os.path.exists(filepath.volumePath):
        pass
    else:
        os.mkdir(filepath.volumePath)

    if os.path.exists(filepath.cpsPath):
        pass
    else:
        os.mkdir(filepath.cpsPath)

    execute_from_command_line(sys.argv)
