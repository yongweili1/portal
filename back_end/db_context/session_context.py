from db_context.models import DjangoSession, NewDjangoSession


class SessionContext(object):
    def create(self, **data):
        try:
            NewDjangoSession.objects.create(**data)
            return True, None
        except Exception as ex:
            return False, ex.message

    def retrieve(self, session_uid=None, new_session_uid=None):
        try:
            if session_uid is not None:
                query = DjangoSession.objects.filter(session_key=session_uid)
            if new_session_uid is not None:
                query = NewDjangoSession.objects.filter(session_key=new_session_uid)
            return query, None
        except Exception as e:
            return None, e.message