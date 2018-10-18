# -*-coding:utf-8-*-
import time

from django.utils.deprecation import MiddlewareMixin


class MySessionMiddleware(MiddlewareMixin):

    def process_request(self, request):
        print("中间件请求")

    def process_response(self, request, response):
        print('中间件响应')
        print(request.session._session)
        if not request.session._session:
            now_time = time.time()
            session_k = str(now_time)
            session_v = 'back_end'
            request.session[session_k] = session_v
            # expire_date = datetime.fromtimestamp(now_time + int(600))

        return response
