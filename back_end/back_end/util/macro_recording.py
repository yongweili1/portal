# -*-coding:utf-8-*-
import functools


class Macro(object):
    def __init__(self):
        self.status = 'start'

    def macro_record(self, func):
        @functools.wraps(func)
        def wrapper(request):
            if request.Get.get('status') == 'start':
                # 开始宏录制
                return func(request)
            elif request.Get.get('status') == 'finish':
                # 关闭宏录制， 保存脚本
                return func(request)
            else:
                return func(request)

        return wrapper
