from db_context.models import DjangoSession, NewDjangoSession


class SessionService(object):
    def __init__(self):
        pass

    def process_session(self, user_ip, session_id):
        session_queryset = DjangoSession.objects.filter(session_key=session_id)
        session_obj = session_queryset[0]
        session_value = session_obj.session_data
        expire_date = session_obj.expire_date

        newsession_obj = NewDjangoSession.objects.filter(session_key=session_id)
        if not newsession_obj:
            data = {
                'client_ip': user_ip,
                'session_key': session_id,
                'session_data': session_value,
                'expire_date': expire_date
            }
            NewDjangoSession.objects.create(**data)
