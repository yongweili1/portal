import json


def cellview_cfg(x_axis, y_axis, o_axis, group, enable_text):
    return {'type': 'view2', 'x_axis': x_axis, 'y_axis': y_axis, 'o_axis': o_axis, 'group': group,
            'enable_text': enable_text}

def get_value(dic, key, default):
    return dic[key] if key in dic else default


class CellViewConfig(object):
    def __init__(self, **kwargs):
        self.enable_text = get_value(kwargs, 'enable_text', True)
        self.top_left_text = get_value(kwargs, 'left_top_text', {'size': [], 'spacing': []})
        self.top_right_text = get_value(kwargs, 'right_top_text', 'zoom:')
        self.bottom_left_text = get_value(kwargs, 'bottom_left_text', 'image type:')
        self.bottom_right_text = get_value(kwargs, 'bottom_right_text', 'color table:')


class ImageViewConfig(object):
    def __init__(self, **kwargs):
        self.name = get_value(kwargs, 'name', 'viewer')
        self.rows = get_value(kwargs, 'rows', 1)
        self.cols = get_value(kwargs, 'cols', 1)
        self.px_per_zoom = get_value(kwargs, 'px_per_zoom', 120)
        self.px_per_percent = get_value(kwargs, 'px_per_percent', 240)
        self.min_zoom_factor = get_value(kwargs, 'min_zoom_factor', 0.01)
        self.max_zoom_factor = get_value(kwargs, 'max_zoom_factor', 25)
        self.alpha_per_delta = get_value(kwargs, 'alpha_per_delta', 0.1)
        self.max_alpha = get_value(kwargs, 'max_alpha', 30)
        self.min_alpha = get_value(kwargs, 'min_alpha', 0)
        self.cross_radius = get_value(kwargs, 'cross_radius', 10)
        self.lm_vis_dist = get_value(kwargs, 'lm_vis_dist', 0.5)
        self.lm_radius = get_value(kwargs, 'lm_radius', 2)
        self.lm_zoom_factor = get_value(kwargs, 'lm_zoom_factor', 0.2)
        self.image_alpha = get_value(kwargs, 'image_alpha', 0.75)

        colormap = {0: [0, 0, 0],
                    1: [255, 0, 0],
                    2: [0, 255, 0],
                    3: [0, 0, 255],
                    4: [255, 255, 0],
                    5: [0, 255, 255],
                    6: [255, 0, 255],
                    7: [255, 255, 255]}
        self.colormap = get_value(kwargs, 'colormap', colormap)
        self.views = get_value(kwargs, 'views', [None] * (self.rows * self.cols))

    def set_subcfg(self, row, col, cfg):
        assert row >= 0 and row < self.rows
        assert col >= 0 and col < self.cols
        self.views[row * self.cols + col] = cfg

    def save_to_json(self, outfile):
        with open(outfile, 'w') as f:
            json.dump(self.__dict__, f, separators=(',', ':'), sort_keys=True, indent=4)


def load_view3_cfg(infile):
    with open(infile, 'r') as f:
        json_data = json.load(f)
    return ImageViewConfig(**json_data)


class CellViewConfigFactory(object):

    @staticmethod
    def cfg_2x2(name):
        opt = {'name': name, 'rows': 2, 'cols': 2}
        config = ImageViewConfig(**opt)

        # transverse view
        v00 = cellview_cfg(x_axis=(1, 0, 0), y_axis=(0, 1, 0), o_axis=(0, 0, 1), group=0, enable_text=True)
        config.set_subcfg(0, 0, v00)

        # saggital view
        v01 = cellview_cfg(x_axis=(0, 1, 0), y_axis=(0, 0, -1), o_axis=(1, 0, 0), group=0, enable_text=True)
        config.set_subcfg(0, 1, v01)

        # coronal view
        v10 = cellview_cfg(x_axis=(1, 0, 0), y_axis=(0, 0, -1), o_axis=(0, 1, 0), group=0, enable_text=True)
        config.set_subcfg(1, 0, v10)

        # coronal view
        v11 = cellview_cfg(x_axis=(1, 0, 0), y_axis=(0, 0, -1), o_axis=(0, 1, 0), group=0, enable_text=True)
        config.set_subcfg(1, 0, v11)

        return config

    @staticmethod
    def cfg_3x1(name):
        opt = {'name': name, 'rows': 3, 'cols': 1}
        config = ImageViewConfig(**opt)

        # transverse view
        v00 = cellview_cfg(x_axis=(1, 0, 0), y_axis=(0, 1, 0), o_axis=(0, 0, 1), group=0, enable_text=True)
        config.set_subcfg(0, 0, v00)

        # saggital view
        v10 = cellview_cfg(x_axis=(0, 1, 0), y_axis=(0, 0, -1), o_axis=(1, 0, 0), group=0, enable_text=False)
        config.set_subcfg(1, 0, v10)

        # coronal view
        v20 = cellview_cfg(x_axis=(1, 0, 0), y_axis=(0, 0, -1), o_axis=(0, 1, 0), group=0, enable_text=False)
        config.set_subcfg(2, 0, v20)

        return config

    @staticmethod
    def cfg_3x1_(name):
        opt = {'name': name, 'rows': 3, 'cols': 1}
        config = ImageViewConfig(**opt)

        # transverse view
        v00 = CellViewConfig().__dict__
        config.set_subcfg(0, 0, v00)

        # saggital view
        v10 = CellViewConfig().__dict__
        config.set_subcfg(1, 0, v10)

        # coronal view
        v20 = CellViewConfig().__dict__
        config.set_subcfg(2, 0, v20)

        return config


if __name__ == '__main__':
    config = CellViewConfigFactory.cfg_3x1_('config_3x1')
    config.save_to_json('./cellview.json')
