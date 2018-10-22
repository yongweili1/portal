# -*-coding:utf-8-*-
import base64
from datetime import datetime
import time

from django.utils.deprecation import MiddlewareMixin
from django.contrib.sessions.middleware import SessionMiddleware
from models import NewDjangoSession, DjangoSession


class MySessionMiddleware(MiddlewareMixin):

    def process_request(self, request):
        pass

    def process_response(self, request, response):

        try:
            sessionid = request.COOKIES['sessionid']
            request.session.allow_save = False
        except:
            sessionid = None

        if sessionid:
            session_queryset = DjangoSession.objects.filter(session_key=sessionid)
            session_obj = session_queryset[0]
            session_value = session_obj.session_data
            expire_date = session_obj.expire_date
            user_ip = request.META.get('REMOTE_ADDR', None)

            newsession_obj = NewDjangoSession.objects.filter(session_key=sessionid)
            if not newsession_obj:
                data = {
                    'client_ip': user_ip,
                    'session_key': sessionid,
                    'session_data': session_value,
                    'expire_date': expire_date
                }
                NewDjangoSession.objects.create(**data)

        sess = request.session._session
        print(sess)
        if not sess:
            now_time = time.time()
            session_k = str(now_time)
            session_v = 'back_end'

            request.session[session_k] = session_v

        return response
