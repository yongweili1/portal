# -*-coding:utf-8-*-
from importlib import import_module
import time

from django.utils.deprecation import MiddlewareMixin
from django.contrib.sessions.middleware import SessionMiddleware


class MySessionMiddleware(MiddlewareMixin):

    def process_request(self, request):
        print("中间件请求")

    def process_response(self, request, response):
        print('中间件响应')
        now_time = time.time()

        # request.session.setdefault(str(now_time), 123)  # 存在则不设置
        response.set_cookie(str(now_time), 123)

        return response
