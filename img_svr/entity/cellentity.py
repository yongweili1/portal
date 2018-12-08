import numpy as np
from model.cellmodel import CellModel, CellsInfo
from model.imagemodel import VolumeInfo, GraphicModel
from model.workflow import GET_CLASS_NAME
from router.routerargs import GraphicState
from router.routerentity import RouterEntity
from scene.camera import SceneCamera
from scene.scene import SliceScene, SceneType
from updater.cellupdater import CellUpdater

from scene.coord import translate_from_screen_to_world
from updater.args import RefreshType
from netbase.c_log import log


class CellEntity(RouterEntity):

    def __init__(self, index=0, accept=False):
        RouterEntity.__init__(self, index)
        self._workflow = None
        self._scene = None
        self._is_maximize = True
        self._updater = CellUpdater(self)
        self._router.accept() if accept else self._router.ignore()

    @property
    def workflow(self):
        return self._workflow

    @workflow.setter
    def workflow(self, wf):
        self._workflow = wf

    def init_scene(self, vol, cam_pos, cam_fov, ww, wl, scene_type):
        if scene_type == SceneType.Slice:
            self._scene = SliceScene()
        elif scene_type == SceneType.VR:
            self._scene = VRScene()
        elif scene_type == SceneType.Mesh:
            self._scene = MeshScene()
        self._scene.volume = vol
        self._scene.init_camera(cam_pos, np.array(vol.center()), cam_fov)
        self._scene.set_window_level(ww, wl)
        # TODO temp fixed page spacing
        self._scene.set_page_spacing(vol.spacing()[2])

        cell_model = CellModel()
        cam = self._scene.camera
        cell_model.default_camera = SceneCamera(cam.look_at, cam.fov, cam.right, cam.up)
        cells_info = self.workflow.get_model(GET_CLASS_NAME(CellsInfo))
        cells_info.cell_models[self._uid] = cell_model

    def get_scene(self):
        return self._scene

    def set_max_view(self, is_max):
        self._is_maximize = is_max

    def set_scene(self, scene):
        self._scene = scene

    def updater(self):
        return self._updater

    def reset_scenes(self):
        pass

    def locate_center(self):
        pass

    def rotate(self, index, angle):
        if self._scene is not None:
            camera_op = self._scene.camera
            norm_axis = np.cross(camera_op.up, camera_op.right)
            camera_op.rotate(norm_axis, angle)

    def pan(self, index, pos_pre, pos_cur):
        if self._scene is not None:
            pt3d_pre = translate_from_screen_to_world(self._scene, pos_pre)
            pt3d_cur = translate_from_screen_to_world(self._scene, pos_cur)
            pt3_shift = pt3d_pre - pt3d_cur
            self._scene.camera.pan(pt3_shift)

    def zoom(self, index, zoom_factor):
        if self._scene is not None:
            self._scene.camera.zoom(zoom_factor)

    def window_(self, index, ww_factor, wl_factor):
        if self._scene is not None:
            ww_op, wl_op = self._scene.get_window_level()
            ww = ww_op * ww_factor
            wl = wl_op + ww_op * wl_factor
            self._scene.set_window_level(ww, wl)

    def page(self, index, delta):
        if self._scene is not None:
            step = delta * self._scene.get_spacing()
            camera_operate = self._scene.camera
            normal_operate = np.cross(camera_operate.up, camera_operate.right)
            camera_operate.look_at -= step * normal_operate
            model_vol = self.workflow.get_model(GET_CLASS_NAME(VolumeInfo))
            model_vol.cursor3d -= step * normal_operate

    def locate(self, index, pt2d):
        if self._scene is not None:
            pt3d = translate_from_screen_to_world(self._scene, pt2d)
            model_vol = self.workflow.get_model(GET_CLASS_NAME(VolumeInfo))
            model_vol.cursor3d = pt3d
            self._scene.camera.look(pt3d)

    def draw_graphic(self, index, graphic_type, pos_pre, pos_cur, state):
        if self._scene is not None:
            pt3d_pre = translate_from_screen_to_world(self._scene, pos_pre)
            pt3d_cur = translate_from_screen_to_world(self._scene, pos_cur)
            overlay_graphic = self.get_overlay(RefreshType.Graphic)
            if state == GraphicState.Initial:
                line = GraphicLine(pos_pre[0], pos_pre[1], pos_cur[0], pos_cur[1])
                overlay_graphic.add_graphic(line)
            else:
                overlay_graphic.get_graphics()[-1].setLine(pos_pre[0], pos_pre[1], pos_cur[0], pos_cur[1])
            if state == GraphicState.Complete:
                model_graphic = self.workflow.get_model(GET_CLASS_NAME(GraphicModel))
                model_graphic.lines.append([pt3d_pre, pt3d_cur])
                dist = np.linalg.norm(pt3d_cur - pt3d_pre)
                log.dev_info('line distance: {}'.format(dist))

    def maximize(self, index, is_max_view):
        self.set_max_view(not is_max_view)
        if is_max_view:
            self.showMaximized()
        else:
            self.showNormal()

    def resize_(self, index, width, height):
        if self._scene is not None:
            self._scene.set_display_size(width, height)
