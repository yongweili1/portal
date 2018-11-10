import os
import ctypes
import threading


class DataChecker:
    _instance_lock = threading.Lock()

    def __init__(self):

        path = os.path.dirname(os.path.abspath(__file__))
        os.chdir(path)
        path = os.path.join(path, "uAIDataLayer.dll")

        try:
            self._lib = ctypes.cdll.LoadLibrary(path)
        except Exception as e:
            print e

    def __new__(cls, *args, **kwargs):
        if not hasattr(DataChecker, "_instance"):
            with DataChecker._instance_lock:
                if not hasattr(DataChecker, "_instance"):
                    DataChecker._instance = object.__new__(cls)
        return DataChecker._instance

    def data_checking(self, series_dir):
        return ctypes.c_char_p(self._lib.data_checking(series_dir)).value


if __name__ == '__main__':
    msg = DataChecker().data_checking(r'E:\to_write\problem_data\2')
    print msg
