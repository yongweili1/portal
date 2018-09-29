#!/usr/bin/env python
import os
import sys
import win32file
from back_end.util.setFilePath import SaveDicomFilePath, SaveVolumeFilePath
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
    if os.path.exists(SaveDicomFilePath.location_1 and SaveDicomFilePath.location_2 and SaveDicomFilePath.location_3):
        pass
    else:
        os.mkdir(SaveDicomFilePath.location_1 and SaveDicomFilePath.location_2 and SaveDicomFilePath.location_3)

    if os.path.exists(SaveVolumeFilePath.volumepath):
        pass
    else:
        os.mkdir(SaveVolumeFilePath.volumepath)

    execute_from_command_line(sys.argv)

    if os.name == 'nt':
        win32file._setmaxstdio(2048)
