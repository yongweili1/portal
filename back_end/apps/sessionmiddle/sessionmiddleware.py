# -*-coding:utf-8-*-
import time

from django.utils.deprecation import MiddlewareMixin
from models import NewDjangoSession, DjangoSession


class MySessionMiddleware(MiddlewareMixin):

    def process_request(self, request):
        requestIP = request.META.get('REMOTE_ADDR', None)
        print(requestIP)

    def process_response(self, request, response):

        try:
            besessionid = request.COOKIES['besessionid']
            print(besessionid)
            request.session.allow_save = False
        except:
            besessionid = None

        if besessionid:
            session_queryset = DjangoSession.objects.filter(session_key=besessionid)
            session_obj = session_queryset[0]
            session_value = session_obj.session_data
            expire_date = session_obj.expire_date
            user_ip = request.META.get('REMOTE_ADDR', None)

            newsession_obj = NewDjangoSession.objects.filter(session_key=besessionid)
            if not newsession_obj:
                data = {
                    'client_ip': user_ip,
                    'session_key': besessionid,
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
            # response.set_cookie('besessionid', session_v)
        # response['Access-Control-Allow-Credentials'] = 'true'
        # print(request.GET.get('Origin'))
        # response['Access-Control-Allow-Origin'] = request.META.get('Origin')

        return response
