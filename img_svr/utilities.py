import base64
import getopt
import sys
from StringIO import StringIO

import numpy as np
from PIL import Image
from enum import Enum


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


class ViewEnum(Enum):
    all = 0,
    transverse = 1,
    saggital = 2,
    coronal = 3


def get_display_view(display_view):
    if display_view == 'transverse':
        return ViewEnum.transverse
    elif display_view == 'saggital':
        return ViewEnum.saggital
    elif display_view == 'coronal':
        return ViewEnum.coronal
    else:
        return ViewEnum.all


def get_focus_view(focus_view):
    if focus_view == 'transverse':
        return ViewEnum.transverse
    elif focus_view == 'saggital':
        return ViewEnum.saggital
    elif focus_view == 'coronal':
        return ViewEnum.coronal


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
