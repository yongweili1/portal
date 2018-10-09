import functools
import os
import sys


class A(object):
    doRecord = False

    def __init__(self):
        pass

    def __call__(self, func):
        self.func = func

        @functools.wraps(func)
        def wrapper2(a, b):
            if self.doRecord:
                file_path1 = sys._getframe().f_code.co_filename
                file_name = os.path.splitext(os.path.basename(file_path1))[0]
                data = 'from %s import %s\n' % (file_name, func.__name__)
                with open('macro_test.py', 'a+') as f:
                    func_name = func.__name__
                    data = data + func_name + str((a, b)) + '\n'
                    print(data)
                    f.write(data)
            else:
                pass
            return func(a, b)
        return wrapper2


@A()
def add(a, b):
    result_add = a + b
    print(result_add)
    return result_add


@A()
def red(a, b):
    result_reduce = a - b
    print(result_reduce)
    return result_reduce


@A()
def mul(a, b):
    result_mul = a * b
    print(result_mul)
    return result_mul


@A()
def divis(a, b):
    result_divis = a / b
    print(result_divis)
    return result_divis


# test = Test()


if __name__ == '__main__':
    A.doRecord = True
    a = test.add(4, 2)
    b = test.add(5, 6)
    c = test.mul(2, 3)
    A.doRecord = False
