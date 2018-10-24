
class BaseUpdater(object):
    def __init__(self, entity):
        self._entity = entity
        self._result = None

    def get_result(self):
        pass

    def update(self, *render_type):
        pass

    def refresh(self, msg, *render_type):
        pass
