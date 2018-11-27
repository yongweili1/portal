from db_context import macro_ctx


class MacroService(object):
    def __init__(self):
        pass

    def create(self, data):
        try:
            return macro_ctx.create(data)
        except Exception as e:
            return False, e.message
