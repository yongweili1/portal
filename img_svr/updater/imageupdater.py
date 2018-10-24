from viewer3.BE.updater.baseupdater import BaseUpdater


class ImageUpdater(BaseUpdater):
    def __init__(self, entity):
        super(ImageUpdater, self).__init__(entity)
        self._result = {'imageview': {}}

    def get_result(self):
        return self._result

    def update(self, *render_type):
        for cell in self._entity.get_children_views():
            if cell.get_scene() is not None:
                cell.updater().update(*render_type)
                self._result['imageview'][cell.uid] = cell.updater().get_result()
