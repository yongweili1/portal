from db_context import session_ctx


class SessionService(object):
    def __init__(self):
        pass

    def process_session(self, user_ip, session_id):
        session_queryset = session_ctx.retrieve(session_uid=session_id)
        session_obj = session_queryset[0]
        session_value = session_obj.session_data
        expire_date = session_obj.expire_date

        newsession_obj = session_ctx.retrieve(new_session_uid=session_id)
        if not newsession_obj:
            data = {
                'client_ip': user_ip,
                'session_key': session_id,
                'session_data': session_value,
                'expire_date': expire_date
            }
            session_ctx.create(**data)
