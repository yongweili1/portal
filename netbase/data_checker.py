import os
import ctypes
import sys
import threading


class DataChecker:
    _instance_lock = threading.Lock()

    def __init__(self):

        lib_path = os.path.dirname(os.path.abspath(__file__))
        os.chdir(lib_path)
        lib_path = os.path.join(lib_path, "uAIDataChecker.dll" if sys.platform == 'win32' else "libuAIDataChecker.so")

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
        return self._lib.data_checking(series_dir)


def main_win32():
    root_dir = r'E:\to_write\problem_data'
    dir_list = []
    for root, dirs, files in os.walk(root_dir):
        if len(files) > 0:
            dir_list.append(root)

    # dir_list.append(r'E:\to_write\problem_data\ImageData\CT\PDB19')

    for p in dir_list:
        msg = DataChecker().data_checking(p)
        print p, "[", "Y" if 0 == msg else 'N', ']'


def main_ubuntu():
    msg = DataChecker().data_checking(r'/home/lyw/work/dcm_checker_test/2')
    print msg

    msg = DataChecker().data_checking(r'/home/lyw/work/dcm_checker_test/T1_GRE_FSP_SAG+C_30')

    print '---------------', msg


if __name__ == '__main__':
    main_win32() if sys.platform == 'win32' else main_ubuntu()
