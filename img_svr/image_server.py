import numpy as np
from md.image3d.python.image3d_tools import slice_nn
from md.image3d.python.image3d_vis import slice_to_bytes, bytes_to_colors
from md.image3d.python.image3d_io import read_image
from md.mdmath.python.rotation3d import axis_angle_to_rotation_matrix
from utilities import get_axis, get_orthogonal_axis, get_two_spacing, get_orthogonal_spacing, ViewEnum, \
    convert_rgba_to_base64, get_view_size, get_view_string, get_focus_view
from scene.coord import translate_from_world_to_screen, translate_from_screen_to_world


class ImageServer(object):
    """Image server"""

    def __init__(self):
        super(ImageServer, self).__init__()

        # All loaded volumes and configs
        self.volumes = {}
        self.configs = {}

        # Current volume and config
        self.vol = None
        self.cfg = {}

        colormap = {1: [255, 0, 0], 2: [0, 255, 0], 3: [0, 0, 255],
                    4: [255, 255, 0], 5: [0, 255, 255], 6: [255, 0, 255]}
        # convert colormap to list of 4-tuples
        self.colormap = np.zeros((256 * 3,), dtype=np.int32)
        for i, item in enumerate(colormap.items()):
            idx = int(item[0])
            assert idx < 256 and idx >= 0
            self.colormap[idx * 3] = item[1][0]
            self.colormap[idx * 3 + 1] = item[1][1]
            self.colormap[idx * 3 + 2] = item[1][2]

        self.px_per_percent = 240
        self.px_per_zoom = 120
        self.min_zoom_factor = 0.01
        self.max_zoom_factor = 25

        # view size
        self.view_size = {
            "transverse": [1150, 728],
            "coronal": [574, 362],
            "saggital": [574, 362]
        }
        self.look_at = {}

    def check_volume_status(self, series_uid):
        return series_uid in self.volumes

    def load_volume(self, volume_path, series_uid):
        """
        Load volume data to memory
        :param volume: volumn data
        :param series_uid: series uid
        :return: True or False
        """
        if volume_path is None:
            return False, 'Volume data is None.'

        im = read_image(volume_path)

        # manage volume data in a dictionary
        self.volumes[series_uid] = im
        # init config info of this volume
        self.configs[series_uid] = {}
        self.configs[series_uid]['size'] = [512, 512, 57]
        self.configs[series_uid]['center'] = im.center()
        self.configs[series_uid]['cursor'] = im.center()
        self.configs[series_uid]['spacing'] = im.spacing()  # [1, 1, 1]
        self.configs[series_uid]['zoom_factor'] = 1
        self.configs[series_uid]['win_center'] = 0
        self.configs[series_uid]['win_width'] = 2000
        self.configs[series_uid]['colormode'] = 0
        self.configs[series_uid]['alpha'] = 1
        self.configs[series_uid]['default_v'] = -1024
        self.configs[series_uid]['transverse_axis'] = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
        self.configs[series_uid]['saggital_axis'] = [[0, 1, 0], [0, 0, -1], [1, 0, 0]]
        self.configs[series_uid]['coronal_axis'] = [[1, 0, 0], [0, 0, -1], [0, 1, 0]]

        # switch current volume to this volume
        self.change_volume(series_uid)

        self.look_at['transverse'] = im.center()
        self.look_at['coronal'] = im.center()
        self.look_at['saggital'] = im.center()

        return True, 'Load succeed.'

    def change_volume(self, series_uid):
        self.vol = self.volumes[series_uid] if series_uid in self.volumes else None
        self.cfg = self.configs[series_uid] if series_uid in self.configs else None
        return self.vol, self.cfg

    def reset_volume(self):
        self.cfg['center'] = self.vol.center()
        self.cfg['cursor'] = self.vol.center()
        self.cfg['zoom_factor'] = 1
        self.cfg['win_center'] = 0
        self.cfg['win_width'] = 2000
        self.cfg['colormode'] = 0
        self.cfg['alpha'] = 1
        self.cfg['transverse_axis'] = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
        self.cfg['saggital_axis'] = [[0, 1, 0], [0, 0, -1], [1, 0, 0]]
        self.cfg['coronal_axis'] = [[1, 0, 0], [0, 0, -1], [0, 1, 0]]

    def unload_volume(self, series_uid):
        if series_uid in self.volumes:
            self.volumes.pop(series_uid)
        if series_uid in self.configs:
            self.configs.pop(series_uid)

    def __get_raw_data(self, view, axis, width, height):
        """
        Get raw data
        :param view:
        :param axis:
        :param width:
        :param height:
        :return:
        """
        pixel_spacing_x, pixel_spacing_y = get_two_spacing(view, self.cfg) * self.cfg['zoom_factor']
        pixel_spacing = [pixel_spacing_x, pixel_spacing_y]
        size = [width, height]
        print(self.cfg['cursor'], axis[0], axis[1], self.cfg['center'], pixel_spacing, size)
        raw_data = slice_nn(self.vol,
                            self.cfg['cursor'],
                            axis[0],
                            axis[1],
                            self.cfg['center'],
                            pixel_spacing,
                            size,
                            self.cfg['default_v'])
        raw_data = raw_data.astype(np.float32)

        return raw_data

    def __get_grayscale_image(self, view, axis, width, height):
        """
        Get grayscale images
        :param view:
        :param axis:
        :param width:
        :param height:
        :return:
        """
        slice = self.__get_raw_data(view, axis, width, height)
        if self.cfg['colormode'] == 5:
            return np.array(slice, dtype=np.uint8)
        else:
            win_center, win_width = self.cfg['win_center'], self.cfg['win_width']
            win_min, win_max = win_center - win_width / 2.0, win_center + win_width / 2.0
            print("win_center:", str(self.cfg['win_center']))
            print("win_width:", str(self.cfg['win_width']))
            return slice_to_bytes(slice, win_min, win_max)

    def __get_rgb_image(self, view, width, height):
        """
        Get rgb image
        :param view:
        :param width:
        :param height:
        :return:
        """
        axis = get_axis(view, self.cfg)
        byte_slice = self.__get_grayscale_image(view, axis, width, height)
        print("colormode:", str(self.cfg['colormode']))
        return bytes_to_colors(byte_slice, self.cfg['colormode'], self.colormap)

    def update_zoom_factor(self, shift):
        # use y-shift to zoom
        pixel_shift = -shift

        if pixel_shift >= 0:
            zoom_factor = 1.0 + pixel_shift * 1.0 / self.px_per_zoom
        else:
            zoom_factor = 1.0 / (1.0 - pixel_shift * 1.0 / self.px_per_zoom)

        zoom_factor *= self.cfg['zoom_factor']
        zoom_factor = min(zoom_factor, self.max_zoom_factor)
        zoom_factor = max(zoom_factor, self.min_zoom_factor)
        self.cfg['zoom_factor'] = zoom_factor

    def update_axis(self, view, angle):
        if view == ViewEnum.transverse:
            # transverse view
            o_axis = np.array(self.cfg['transverse_axis'][2])
            rot_mat = axis_angle_to_rotation_matrix(o_axis, angle)
            x_axis = rot_mat.dot(self.cfg['transverse_axis'][0])
            y_axis = rot_mat.dot(self.cfg['transverse_axis'][1])
            self.cfg['transverse_axis'][2] = get_orthogonal_axis(x_axis, y_axis)
            self.cfg['saggital_axis'][2] = get_orthogonal_axis(y_axis, self.cfg['saggital_axis'][1])
            self.cfg['coronal_axis'][2] = get_orthogonal_axis(x_axis, self.cfg['coronal_axis'][1])

        elif view == ViewEnum.saggital:
            # sagittal view
            o_axis = np.array(self.cfg['saggital_axis'][2])
            rot_mat = axis_angle_to_rotation_matrix(o_axis, angle)
            x_axis = rot_mat.dot(self.cfg['saggital_axis'][0])
            y_axis = rot_mat.dot(self.cfg['saggital_axis'][1])
            self.cfg['saggital_axis'][2] = get_orthogonal_axis(x_axis, y_axis)
            self.cfg['transverse_axis'][2] = get_orthogonal_axis(self.cfg['transverse_axis'][0], x_axis)
            self.cfg['coronal_axis'][2] = get_orthogonal_axis(self.cfg['coronal_axis'][0], y_axis)

        elif view == ViewEnum.coronal:
            # coronal view
            o_axis = np.array(self.cfg['coronal_axis'][2])
            rot_mat = axis_angle_to_rotation_matrix(o_axis, angle)
            x_axis = rot_mat.dot(self.cfg['coronal_axis'][0])
            y_axis = rot_mat.dot(self.cfg['coronal_axis'][1])
            self.cfg['coronal_axis'][2] = get_orthogonal_axis(x_axis, y_axis)
            self.cfg['transverse_axis'][2] = get_orthogonal_axis(x_axis, self.cfg['transverse_axis'][1])
            self.cfg['saggital_axis'][2] = get_orthogonal_axis(self.cfg['saggital_axis'][0], y_axis)

    def update_center(self, shift):
        self.cfg['center'] -= shift

    def set_cursor(self, cursor):
        self.cfg['cursor'] = cursor

    def update_cursor(self, view, delta):
        shift = get_orthogonal_spacing(view, self.cfg) * delta
        axis = get_axis(view, self.cfg)
        self.cfg['cursor'] = self.cfg['cursor'] + axis[2] * shift
        return self.cfg['cursor']

    def set_look_at(self, *args):
        self.look_at['transverse'] = args[0]
        self.look_at['coronal'] = args[1]
        self.look_at['saggital'] = args[2]

    def update_look_at(self, view, delta):
        shift = get_orthogonal_spacing(view, self.cfg) * delta
        axis = get_axis(view, self.cfg)
        view_string = get_view_string(view)
        self.look_at[view_string] = self.look_at[view_string] + axis[2] * shift
        return self.look_at[view_string]

    def set_colormode(self, colormode):
        self.cfg['colormode'] = colormode

    def set_wcww(self, shift):
        x, y = -shift[0], shift[1]

        # adjust window width by percents
        if y >= 0:
            ww_factor = 1.0 + y * 1.0 / self.px_per_percent
        else:
            ww_factor = 1.0 / (1.0 - y * 1.0 / self.px_per_percent)
        ww = self.cfg['win_width'] * ww_factor
        if ww < 1:
            ww = 1

        # adjust window center using window width
        percent = x * 1.0 / self.px_per_percent
        wc = self.cfg['win_center'] + self.cfg['win_width'] * percent

        self.cfg['win_center'] = wc
        self.cfg['win_width'] = ww

    def get_images(self, display_view, width, height):
        data = {}

        if display_view == ViewEnum.transverse or display_view == ViewEnum.all:
            transverse = self.__get_rgb_image(ViewEnum.transverse, self.view_size['transverse'][0],
                                              self.view_size['transverse'][1])
            data['transverse'] = convert_rgba_to_base64(transverse, 'PNG')

        if display_view == ViewEnum.saggital or display_view == ViewEnum.all:
            saggital = self.__get_rgb_image(ViewEnum.saggital, self.view_size['saggital'][0],
                                            self.view_size['saggital'][1])
            data['saggital'] = convert_rgba_to_base64(saggital, 'PNG')

        if display_view == ViewEnum.coronal or display_view == ViewEnum.all:
            coronal = self.__get_rgb_image(ViewEnum.coronal, self.view_size['coronal'][0], self.view_size['coronal'][1])
            data['coronal'] = convert_rgba_to_base64(coronal, 'PNG')

        return data

    def dimension_translate(self, trans_para):
        if trans_para['trans_direct_flag'] == "world2screen":
            point_2d = {}
            point_2d['transPosition'] = translate_from_world_to_screen(self.cfg['transverse_axis'][0],
                                                                        self.cfg['transverse_axis'][1], self.look_at['transverse'],
                                                                        self.view_size['transverse'],
                                                                        1,
                                                                       trans_para['point_3d']).tolist()
            point_2d['cronPosition'] = translate_from_world_to_screen(self.cfg['coronal_axis'][0],
                                                                       self.cfg['coronal_axis'][1], self.look_at['coronal'],
                                                                       self.view_size['coronal'],
                                                                       self.cfg['spacing'][0],
                                                                      trans_para['point_3d']).tolist()
            point_2d['sagPosition'] = translate_from_world_to_screen(self.cfg['saggital_axis'][0],
                                                                      self.cfg['saggital_axis'][1], self.look_at['saggital'],
                                                                      self.view_size['saggital'],
                                                                      self.cfg['spacing'][1],
                                                                     trans_para['point_3d']).tolist()

            return point_2d
        else:
            if trans_para['focus_view'] == 'transverse':
                point_3d = translate_from_screen_to_world(self.cfg['transverse_axis'][0],
                                                          self.cfg['transverse_axis'][1], self.look_at['transverse'],
                                                          self.view_size['transverse'],
                                                          1,
                                                          trans_para['point_2d']).tolist()
            elif trans_para['focus_view'] == 'coronal':
                point_3d = translate_from_screen_to_world(self.cfg['coronal_axis'][0],
                                                          self.cfg['coronal_axis'][1], self.look_at['coronal'],
                                                          self.view_size['coronal'],
                                                          self.cfg['spacing'][0],
                                                          trans_para['point_2d']).tolist()
            else:
                point_3d = translate_from_screen_to_world(self.cfg['saggital_axis'][0],
                                                          self.cfg['saggital_axis'][1], self.look_at['saggital'],
                                                          self.view_size['saggital'],
                                                          self.cfg['spacing'][1],
                                                          trans_para['point_2d']).tolist()
            return point_3d

    def set_view_size(self, **kwargs):
        self.view_size['transverse'] = kwargs['transverse']
        self.view_size['coronal'] = kwargs['coronal']
        self.view_size['saggital'] = kwargs['saggital']

    def get_multi_images(self, display_view_array):
        data = {}
        for display_view in display_view_array:
            width, height = self.view_size[display_view]
            display_view = get_focus_view(display_view)
            img = self.get_images(display_view, width, height)
            data.update(img)

        return data


