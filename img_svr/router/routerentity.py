from router.duplexrouter import DuplexRouter


class RouterEntity:
    """
    an entity which send/receive command through router, such as cellview, imageview and multimageview

    """
    def __init__(self, router_id):
        self._uid = router_id
        self._router = DuplexRouter(router_id, self)

    @property
    def uid(self):
        return self._uid

    @property
    def router(self):
        return self._router

    def _set_action(self, path, act, btn=1):
        self.set_action(act, btn)

    def _reset_scenes(self, path):
        self.reset_scenes()

    def _locate_center(self, path):
        self.locate_center()

    def _rotate(self, path, angle):
        self.rotate(path[1], angle)

    def _pan(self, path, pos_pre, pos_cur):
        self.pan(path[1], pos_pre, pos_cur)

    def _zoom(self, path, zoom_factor):
        self.zoom(path[1], zoom_factor)

    def _window(self, path, ww_factor, wl_factor):
        self.window_(path[1], ww_factor, wl_factor)

    def _page(self, path, delta):
        self.page(path[1], delta)

    def _locate(self, path, pt2d):
        self.locate(path[1], pt2d)

    def _draw_graphic(self, path, graphic_type, pos_pre, pos_cur, need_created):
        self.draw_graphic(path[1], graphic_type, pos_pre, pos_cur, need_created)

    def _maximize(self, path, is_max_view):
        self.maximize(path[1], is_max_view)

    def _resize(self, path, width, height):
        self.resize_(path[1], width, height)

    def set_action(self, act, btn):
        pass

    def reset_scenes(self):
        pass

    def locate_center(self):
        pass

    def rotate(self, index, angle):
        pass

    def pan(self, index, pos_pre, pos_cur):
        pass

    def zoom(self, index, zoom_factor):
        pass

    def window_(self, index, ww_factor, wl_factor):
        pass

    def page(self, index, delta):
        pass

    def locate(self, index, pt2d):
        pass

    def draw_graphic(self, index, graphic_type, pos_pre, pos_cur, need_created):
        pass

    def maximize(self, index, is_max_view):
        pass

    def resize_(self, index, width, height):
        pass

