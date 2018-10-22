# -*- coding: utf-8 -*-
from datetime import datetime

import numpy as np
from enum import Enum

from md.image3d.python.image3d_tools import slice_nn
from md.image3d.python.image3d_vis import slice_to_bytes, bytes_to_colors, multi_image_alpha_blend

from scene.camera import SceneCamera


class SceneType(Enum):
    Slice = 0
    VR = 1
    Mesh = 2


class ColorMode(Enum):
    Gray = 0
    Red = 1
    Green = 2
    Blue = 3
    Jet = 4
    Map = 5


class CameraPos(Enum):
    Transverse = 0
    Sagittal = 1
    Coronal = 2


class Orientation(Enum):
    Horizontal = 0
    Vertical = 1


class SceneBase(object):
    def __init__(self):
        self._image3d = None
        self._width = 512
        self._height = 512
        self._spacing = 1
        self._camera = None
        self._window_width = 0
        self._window_level = 0
        self.__camera_default_pos = None

    def render(self):
        pass

    @property
    def volume(self):
        return self._image3d

    @volume.setter
    def volume(self, img3d):
        self._image3d = img3d

    @property
    def camera(self):
        return self._camera

    @camera.setter
    def camera(self, camera):
        self._camera = camera

    def get_default_pos(self):
        return self.__camera_default_pos

    def get_display_size(self):
        return [self._width, self._height]

    def get_spacing(self):
        return self._spacing

    def get_view_size(self):
        return [self._width * self._spacing, self._height * self._spacing]

    def get_window_level(self):
        return self._window_width, self._window_level

    def set_display_size(self, width, height):
        self._width = width
        self._height = height

    def set_window_level(self, ww, wl):
        self._window_width = ww
        self._window_level = wl

    def init_camera(self, pos, center, fov):
        self._camera = SceneCamera(center, fov)
        self.__camera_default_pos = pos
        if pos == CameraPos.Transverse:
            self.camera.right = [1, 0, 0]
            self.camera.up = [0, 1, 0]
        elif pos == CameraPos.Coronal:
            self.camera.right = [1, 0, 0]
            self.camera.up = [0, 0, -1]
        else:
            self.camera.right = [0, 1, 0]
            self.camera.up = [0, 0, -1]


class SliceScene(SceneBase):
    """
    a scene with camera to generate an slice image
    this determines how to display raw data with ww/wl, colormap and others

    """

    def __init__(self):
        super(SliceScene, self).__init__()
        self.__color_mode = ColorMode.Gray
        self.__color_map = None
        self.__default_v = -1024
        self.__voi_list = []

    def render(self):
        print 'render start'
        begin = datetime.now()

        self._spacing = max(self._camera.fov[0] / float(self._width), self._camera.fov[1] / float(self._height))
        print 'spacing: {}, fov: {}, size: {}'.format(self._spacing, self._camera.fov, [self._width, self._height])
        raw_data = slice_nn(self._image3d, self._camera.look_at,
                            self._camera.right, self._camera.up,
                            self._camera.look_at, [self._spacing, self._spacing],
                            [self._width, self._height], self.__default_v)

        if self.__color_mode is not ColorMode.Map:
            win_min = self._window_level - self._window_width / 2
            win_max = self._window_level + self._window_width / 2
            byte_data = slice_to_bytes(raw_data, win_min, win_max)
            color_data = bytes_to_colors(byte_data, self.__color_mode.value, None)
        else:
            color_data = bytes_to_colors(raw_data, self.__color_mode.value, self.__color_map)

        voi_color = [(255, 0, 0), (0, 255, 0), (0, 0, 255), (255, 255, 0), (255, 0, 255), (0, 255, 255)]
        for i, voi in enumerate(self.__voi_list):
            voi.from_numpy(voi.to_numpy(), np.uint8)
            voi_slice = slice_nn(voi, self._camera.look_at,
                                 self._camera.right, self._camera.up,
                                 self._camera.look_at, [self._spacing, self._spacing],
                                 [self._width, self._height], self.__default_v)
            import cv2
            _, thresh = cv2.threshold(voi_slice, 0, 255, cv2.THRESH_BINARY)
            _, contours, _ = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
            cv2.drawContours(color_data, contours, -1, voi_color[i], 2)

        end = datetime.now()
        print 'render slice time: {}'.format(end - begin)
        print 'render end'
        return color_data

    def set_color_mode(self, mode):
        self.__color_mode = mode

    def set_color_map(self, mapping):
        self.__color_map = np.zeros((256 * 3,), dtype=np.int32)
        for index, value in mapping.items():
            assert 0 <= index < 255
            self.__color_map[index * 3] = value[0]
            self.__color_map[index * 3 + 1] = value[1]
            self.__color_map[index * 3 + 2] = value[2]

    def add_voi(self, *voi):
        for v in voi:
            self.__voi_list.append(v)


class FusionScene(SliceScene):
    def __init__(self):
        super(FusionScene, self).__init__()
        self.__multi_image3d = []
        self.__alphas = []

    def render(self):
        slices = np.zeros((len(self.__multi_image3d), self._height, self._width, 3), dtype=np.uint8)
        # alphas = np.zeros((len(self.__images3d,)), dtype=np.double)
        for i, vol in enumerate(self.__multi_image3d):
            self._image3d = vol
            slice_color = SliceScene.render(self)
            slices[i] = slice_color
        alphas = np.array(self.__alphas, dtype=np.double)
        slice_fusion = multi_image_alpha_blend(slices, alphas)
        return slice_fusion

    @property
    def volumes(self):
        return self.__multi_image3d

    @volumes.setter
    def volumes(self, vols):
        self.__multi_image3d = vols

    @property
    def alphas(self):
        return self.__alphas

    @alphas.setter
    def alphas(self, alphas):
        self.__alphas = alphas