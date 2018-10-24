from viewer3.args import RefreshType
from viewer3.BE.model.workflow import WorkflowController
from viewer3.BE.router.routerentity import RouterEntity
from viewer3.BE.updater.multimageupdater import MultiImageUpdater


class MultiImageEntity(RouterEntity):

    def __init__(self, index=0, accept=True):
        RouterEntity.__init__(self, index)
        self._imageviews = []
        self._workflow_controller = WorkflowController()
        self._updater = MultiImageUpdater(self)
        self._router.accept() if accept else self._router.ignore()

    def get_children_views(self):
        return self._imageviews

    def updater(self):
        return self._updater

    def reset_scenes(self):
        for imageview in self._imageviews:
            imageview.reset_scenes()

    def locate_center(self):
        for imageview in self._imageviews:
            imageview.locate_center()

    def rotate(self, index, angle):
        for imageview in self._imageviews:
            imageview.rotate(index, angle)

    def pan(self, index, pos_pre, pos_cur):
        for imageview in self._imageviews:
            imageview.pan(index, pos_pre, pos_cur)

    def zoom(self, index, zoom_factor):
        """ zoom view using y-direction mouse shift """
        for imageview in self._imageviews:
            imageview.zoom(index, zoom_factor)

    def window_(self, index, ww_factor, wl_factor):
        for imageview in self._imageviews:
            imageview.window_(index, ww_factor, wl_factor)

    def page(self, index, delta):
        for imageview in self._imageviews:
            imageview.page(index, delta)

    def locate(self, index, pt2d):
        for imageview in self._imageviews:
            imageview.locate(index, pt2d)

    def draw_graphic(self, index, graphic_type, pos_pre, pos_cur, state):
        for imageview in self._imageviews:
            imageview.draw_graphic(index, graphic_type, pos_pre, pos_cur, state)

    def maximize(self, index, is_max_view):
        for imageview in self._imageviews:
            imageview.maximize(index, is_max_view)
