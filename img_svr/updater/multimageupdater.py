from updater.baseupdater import BaseUpdater


class MultiImageUpdater(BaseUpdater):
    def __init__(self, entity):
        super(MultiImageUpdater, self).__init__(entity)
        self._result = {'multimageview': {}}

    def get_result(self):
        return self._result

    def update(self, *render_type):
        for imageview in self._entity.get_children_views():
            imageview.updater().update(*render_type)
            self._result['multimageview'][imageview.uid] = imageview.updater().get_result()
