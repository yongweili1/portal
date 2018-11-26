# -*-coding:utf-8-*-
import time

from django.utils.deprecation import MiddlewareMixin

from service import session_svc


class MySessionMiddleware(MiddlewareMixin):

    def process_request(self, request):
        # requestIP = request.META.get('REMOTE_ADDR', None)
        # print(requestIP)
        pass

    def process_response(self, request, response):

        try:
            besessionid = request.COOKIES['besessionid']
            print(besessionid)
            request.session.allow_save = False
        except:
            besessionid = None

        user_ip = request.META.get('REMOTE_ADDR', None)

        if besessionid:
            session_svc.process_session(user_ip, besessionid)

        sess = request.session._session
        print(sess)

        if not sess:
            now_time = time.time()
            session_k = str(now_time)
            session_v = 'back_end'

            request.session[session_k] = session_v

        return response
