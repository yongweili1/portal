# -*- coding: utf-8 -*-
from datetime import datetime

import numpy as np
from enum import Enum
from md.image3d.python.image3d_tools import slice_nn
from md.image3d.python.image3d_vis import slice_to_bytes, bytes_to_colors, multi_image_alpha_blend

from camera import SceneCamera
from netbase.c_log import log
from utilities import execute_time


class SceneType(Enum):
    Slice = 0
    Fusion = 1
    VR = 2
    Mesh = 3


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
        self._image_result = None
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

    def get_image_result(self):
        return self._image_result

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
        self.__contours_list = []
        self.__voi_list = []
        self._page_spacing = 1

    @execute_time
    def render(self):
        log.dev_info('render start')
        begin = datetime.now()

        self._spacing = max(self._camera.fov[0] / float(self._width), self._camera.fov[1] / float(self._height))
        log.dev_info('spacing: {}, fov: {}, size: {}, wwwl: {}'.format(
            self._spacing, self._camera.fov, [self._width, self._height], self.get_window_level()))
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

        log.dev_info('render finish')
        self._image_result = color_data
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

    def get_contours(self):
        return self.__contours_list

    def add_voi(self, *voi):
        for v in voi:
            self.__voi_list.append(v)

    def set_page_spacing(self, page_spacing):
        self._page_spacing = page_spacing

    def get_page_spacing(self):
        return self._page_spacing


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
