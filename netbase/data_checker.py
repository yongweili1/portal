import os
import ctypes
import sys
import threading


class DataChecker:
    _instance_lock = threading.Lock()

    def __init__(self):

        lib_path = os.path.dirname(os.path.abspath(__file__))
        os.chdir(lib_path)
        lib_path = os.path.join(lib_path, "uAIDataChecker.dll" if sys.platform == 'win32' else "uAIDataChecker.so")

        try:
            self._lib = ctypes.cdll.LoadLibrary(lib_path)
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
    path = (r'E:\to_write\2017_12_28_12_57_46',
            r'E:\to_write\problem_data\2',
            r'E:\to_write\problem_data\T1_GRE_FSP_SAG+C_30',
            r'E:\to_write\segmentation\test_data\577760_1.3.12.2.1107.5.1.4.64606.30000018050818223536400006643\dicom'
            )
    for p in path:
        msg = DataChecker().data_checking(p)
        print p, "[", msg if msg else 'no problem data', ']'
