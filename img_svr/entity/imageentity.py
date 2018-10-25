# -*- coding: utf-8 -*-
import numpy as np
from md import Image3d

from args import RefreshType
from model.cellmodel import CellsInfo
from model.imagemodel import VolumeInfo, GraphicModel
from model.workflow import GET_CLASS_NAME, Workflow
from router.routerargs import GraphicType
from router.routerentity import RouterEntity
from scene.camera import SceneCamera
from scene.coord import translate_from_screen_to_world
from scene.scene import CameraPos, SceneType
from updater.imageupdater import ImageUpdater


class ImageEntity(RouterEntity):

    def __init__(self, index, accept):
        RouterEntity.__init__(self, index)
        self._volume = None
        self._cellviews = []
        self._workflow = Workflow(index)
        self._updater = ImageUpdater(self)
        self._router.accept() if accept else self._router.ignore()

    @property
    def workflow(self):
        return self._workflow

    @workflow.setter
    def workflow(self, workflow):
        self._workflow = workflow

    def set_volume(self, volume):
        assert isinstance(volume, Image3d)
        self._volume = volume
        volume_model = self._workflow.get_model(GET_CLASS_NAME(VolumeInfo))
        volume_model.cursor3d = np.array(volume.center())
        volume_model.default_cursor3d = np.array(volume.center())

    def init_default_scenes(self, volume):
        """
        # 第一种设置scene的方法，init后再设置scene的属性
        volume = self._volume
        self._cellviews[0].init_scene(volume, CameraPos.Transverse, [350, 350], 126, -1015)
        self._cellviews[0].get_scene().set_color_mode(ColorMode.Jet)

        # 第二种设置scene的方法，构造FusionScene再set_scene
        scene0x1 = FusionScene()
        scene0x1.volumes = [volume]
        scene0x1.alphas = len(scene0x1.volumes) * [1]
        scene0x1.init_camera(CameraPos.Transverse, np.array(volume.center()), [350, 350])
        scene0x1.set_window_level(126, -1015)
        self._cellviews[1].set_scene(scene0x1)

        # 第三种设置scene的方法， 使用init_scene方法即可
        self._cellviews[2].init_scene(volume, CameraPos.Sagittal, [350, 350], 126, -1015)

        # 第四种设置scene的方法，构造SliceScene再set_scene
        scene1x1 = SliceScene()
        scene1x1.volume = volume
        scene1x1.init_camera(CameraPos.Coronal, np.array(volume.center()), [350, 350])
        scene1x1.set_window_level(6855, 2404)
        self._cellviews[3].set_scene(scene1x1)

        """
        assert isinstance(volume, Image3d)
        self._volume = volume
        volume_model = self._workflow.get_model(GET_CLASS_NAME(VolumeInfo))
        volume_model.cursor3d = np.array(volume.center())
        volume_model.default_cursor3d = np.array(volume.center())

        num_cells = len(self._cellviews)
        if num_cells > 0:
            self._cellviews[0].init_scene(volume, CameraPos.Transverse, [350, 350], 2000, 0, SceneType.Slice)
        if num_cells > 1:
            self._cellviews[1].init_scene(volume, CameraPos.Coronal, [350, 350], 2000, 0, SceneType.Slice)
        if num_cells > 2:
            self._cellviews[2].init_scene(volume, CameraPos.Sagittal, [350, 350], 2000, 0, SceneType.Slice)

    def get_children_views(self):
        return self._cellviews

    def add_child_entity(self, cell_entity):
        self._cellviews.append(cell_entity)
        self._workflow.add_cellview(cell_entity)
        self._router.set_sender(cell_entity.router)

    def updater(self):
        return self._updater

    def reset_scenes(self):
        model_volume = self.workflow.get_model(GET_CLASS_NAME(VolumeInfo))
        model_volume.cursor3d = np.array(model_volume.default_cursor3d)
        cells_info = self.workflow.get_model(GET_CLASS_NAME(CellsInfo))
        for cell in self._cellviews:
            scene = cell.get_scene()
            if scene is not None:
                cam = cells_info.cell_models[cell.uid].default_camera
                scene.camera = SceneCamera(cam.look_at, cam.fov, cam.right, cam.up)

    def locate_center(self):
        model_volume = self.workflow.get_model(GET_CLASS_NAME(VolumeInfo))
        for cell in self._cellviews:
            scene = cell.get_scene()
            if scene is not None:
                scene.camera.look_at = np.array(model_volume.cursor3d)

    def rotate(self, index, angle):
        if self._cellviews is not None and len(self._cellviews) > index:
            scene_operate = self._cellviews[index].get_scene()
            camera_op = scene_operate.camera
            norm_axis = np.cross(camera_op.up, camera_op.right)
            for cell in self._cellviews:
                scene = cell.get_scene()
                if scene is not None:
                    scene.camera.rotate(norm_axis, angle)

    def pan(self, index, pos_pre, pos_cur):
        if self._cellviews is not None and len(self._cellviews) > index:
            scene_operate = self._cellviews[index].get_scene()
            pt3d_pre = translate_from_screen_to_world(scene_operate, pos_pre)
            pt3d_cur = translate_from_screen_to_world(scene_operate, pos_cur)
            pt3_shift = pt3d_pre - pt3d_cur
            for cell in self._cellviews:
                scene = cell.get_scene()
                if scene is not None:
                    scene.camera.pan(pt3_shift)

    def zoom(self, index, zoom_factor):
        """ zoom view using y-direction mouse shift """
        for cell in self._cellviews:
            scene = cell.get_scene()
            if scene is not None:
                scene.camera.zoom(zoom_factor)

    def window_(self, index, ww_factor, wl_factor):
        if self._cellviews is not None and len(self._cellviews) > index:
            scene_operate = self._cellviews[index].get_scene()
            ww_op, wl_op = scene_operate.get_window_level()
            for cell in self._cellviews:
                scene = cell.get_scene()
                if scene is not None:
                    ww = ww_op * ww_factor
                    wl = wl_op + ww_op * wl_factor
                    scene.set_window_level(ww, wl)

    def page(self, index, delta):
        if self._cellviews is not None and len(self._cellviews) > index:
            cell_operate = self._cellviews[index]
            scene_operate = cell_operate.get_scene()
            spacing_operate = scene_operate.get_spacing()
            step = delta * spacing_operate
            camera_operate = scene_operate.camera
            normal_operate = np.cross(camera_operate.up, camera_operate.right)
            camera_operate.look_at -= step * normal_operate
            model_volume = self.workflow.get_model(GET_CLASS_NAME(VolumeInfo))
            model_volume.cursor3d -= step * normal_operate

    def locate(self, index, pt2d):
        if self._cellviews is not None and len(self._cellviews) > index:
            scene_operate = self._cellviews[index].get_scene()
            pt3d = translate_from_screen_to_world(scene_operate, pt2d)
            model_volume = self.workflow.get_model(GET_CLASS_NAME(VolumeInfo))
            model_volume.cursor3d = pt3d
            for cell in self._cellviews:
                scene = cell.get_scene()
                if scene is not None:
                    scene.camera.look(pt3d)

    def draw_graphic(self, index, graphic_type, pos_pre, pos_cur, uid):
        if self._cellviews is not None and len(self._cellviews) > index:
            cell_operate = self._cellviews[index]
            scene_operate = cell_operate.get_scene()
            pt3d_pre = translate_from_screen_to_world(scene_operate, pos_pre)
            pt3d_cur = translate_from_screen_to_world(scene_operate, pos_cur)
            if graphic_type == GraphicType.ProLines:
                model_graphic = self.workflow.get_model(GET_CLASS_NAME(GraphicModel))
                model_graphic.lines[uid] = (tuple(pt3d_pre), tuple(pt3d_cur))
                dist = np.linalg.norm(pt3d_cur - pt3d_pre)
                print 'line distance: {}'.format(dist)
            # elif graphic_type == GraphicType.Circle:
            #     overlay_graphic = cell_operate.get_overlay(RefreshType.Graphic)
            #     shift = (pos_cur[0] - pos_pre[0], pos_cur[1] - pos_pre[1])
            #     radius = np.linalg.norm(shift)
            #     center = (pos_pre[0], pos_pre[1])
            #     if state == GraphicState.Initial:
            #         circle = GraphicCircle()
            #         overlay_graphic.add_circle(circle)
            #     elif state == GraphicState.Drawing:
            #         overlay_graphic.get_circles()[-1].setCircle(center, radius)
            #     else:
            #         model_graphic = self.workflow.get_model(GET_CLASS_NAME(GraphicModel))
            #         cell_circle = model_graphic.circle.get(index)
            #         if cell_circle is not None:
            #             cell_circle.append((pt3d_pre, pt3d_cur))
            #         else:
            #             model_graphic.circle[index] = [(pt3d_pre, pt3d_cur)]

    def maximize(self, index, is_max_view):
        if self._cellviews is not None and len(self._cellviews) > index:
            cell_operate = self._cellviews[index]
            cell_operate.set_max_view(not is_max_view)
            if is_max_view:
                for i, cell in enumerate(self._cellviews):
                    if not i == index:
                        cell.hide()

            else:
                for i, cell in enumerate(self._cellviews):
                    if not i == index:
                        cell.show()
