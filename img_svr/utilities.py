import base64
import getopt
import sys
from StringIO import StringIO

import numpy as np
from PIL import Image
from enum import IntEnum


def get_args(argv):
    kwargs = {}
    kwargs['host'] = '0.0.0.0'
    kwargs['port'] = 0
    try:
        opts, args = getopt.getopt(argv, "h:p:", ["host=", "port="])
    except getopt.GetoptError:
        print 'server.py -h <host> -p <port>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            kwargs['host'] = arg
        elif opt == '-p':
            kwargs['port'] = int(arg)
    return kwargs


class ViewEnum(IntEnum):
    transverse = 0,
    coronal = 1,
    saggital = 2,
    trans_and_cro = 3,
    trans_and_sag = 4,
    cro_and_sag = 5,
    all = 6


def get_view_index(view_name):
    if view_name == 'transverse':
        return ViewEnum['transverse'].value
    elif view_name == 'saggital':
        return ViewEnum['saggital'].value
    elif view_name == 'coronal':
        return ViewEnum['coronal'].value
    elif view_name == 'transverse,coronal':
        return ViewEnum['trans_and_cro'].value
    elif view_name == 'transverse,saggital':
        return ViewEnum['trans_and_sag'].value
    elif view_name == 'coronal,saggital':
        return ViewEnum['cro_and_sag'].value
    else:
        return ViewEnum['all'].value


def get_axis(view, cfg):
    if view == ViewEnum.transverse:
        return np.array(cfg['transverse_axis'], dtype=np.float)
    elif view == ViewEnum.saggital:
        return np.array(cfg['saggital_axis'], dtype=np.float)
    elif view == ViewEnum.coronal:
        return np.array(cfg['coronal_axis'], dtype=np.float)


def get_orthogonal_axis(x_axis, y_axis):
    x = np.array(x_axis, dtype=np.double)
    y = np.array(y_axis, dtype=np.double)
    return np.cross(x, y)


def get_orthogonal_spacing(view, cfg):
    if view == ViewEnum.transverse:
        return cfg['spacing'][2]
    elif view == ViewEnum.saggital:
        return cfg['spacing'][0]
    elif view == ViewEnum.coronal:
        return cfg['spacing'][1]


def get_view_size(view, size):
    if view == ViewEnum.transverse:
        return size['transverse']
    elif view == ViewEnum.saggital:
        return size['saggital']
    elif view == ViewEnum.coronal:
        return size['coronal']


def get_view_string(view):
    if view == ViewEnum.transverse:
        return 'transverse'
    elif view == ViewEnum.saggital:
        return 'saggital'
    elif view == ViewEnum.coronal:
        return 'coronal'


def get_two_spacing(view, cfg):
    if view == ViewEnum.transverse:
        return cfg['spacing'][0], cfg['spacing'][1]
    elif view == ViewEnum.saggital:
        return cfg['spacing'][1], 1
    elif view == ViewEnum.coronal:
        return cfg['spacing'][0], 1


def get_spacing(view, cfg):
    if view == ViewEnum.transverse:
        return cfg['spacing'][0]
    elif view == ViewEnum.saggital:
        return cfg['spacing'][1]
    elif view == ViewEnum.coronal:
        return cfg['spacing'][2]


def convert_rgba_to_base64(rgba, format):
    im = Image.fromarray(rgba)
    output_buffer = StringIO()
    im.save(output_buffer, format=format)
    binary_data = output_buffer.getvalue()
    base64_data = base64.b64encode(binary_data)
    return base64_data


def view_filter(views, filter_image):
    """
    filter no need image data to improve performance
    :param views:
    :param filter_image_list:
    :return:
    """
    filter_image_list = []
    result = {}
    if isinstance(filter_image, list):
        filter_image_list = filter_image
    else:
        filter_image_list.append(filter_image)
    for (view_key, view_value) in views.items():
        if view_key in filter_image_list:
            view_value['image'] = {}
        result[view_key] = view_value

    return result


def get_page_filter_view(focus_view):
        if focus_view == 0:
            return [1, 2]
        elif focus_view == 1:
            return [0, 2]
        elif focus_view == 2:
            return [0, 1]
        else:
            print("focus_view index error")
