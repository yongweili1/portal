# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import time

from entity.cellentity import CellEntity
from image_server import ImageServer
from message import response
from updater.args import RefreshType
from utilities import get_view_index, get_orthogonal_spacing, ViewEnum, get_view_index, get_page_filter_view, \
    view_filter, string_int_trans, cal_angle
from entity.imageentity import ImageEntity
import md.image3d.python.image3d_io as cio

import json
import datetime


class Command(object):
    def __init__(self):
        self.commands = {}

    def register(self, command):
        print('Register command {} succeed.'.format(command))
        def wrapper(func):
            self.commands[command] = func
            print('inner command {} succeed.'.format(command))
            return func
        return wrapper


# single instance of Command
command = Command()


# single instance of ImageServer
server = ImageServer()

imageentity = ImageEntity(0, True)


@command.register('greeting')
def greeting(**kwargs):
    """
    Say hi to the world
    """
    return response('Hi developer, good luck!')


@command.register('status')
def status(**kwargs):
    try:
        seriesuid = kwargs['seriesuid']
    except Exception as err:
        return response(success=False, message='Invalid parameters.')

    rst = server.check_volume_status(seriesuid)
    return response(str(rst))


@command.register('load')
def load(**kwargs):
    """
    Load volume data and managed in memory
    :param volume: volume data
    :param seriesuid: series uid
    :return:
    """
    try:
        volume_path = kwargs['volume_path']
        seriesuid = kwargs['seriesuid']
    except Exception as err:
        return response(success=False, message='Invalid parameters.')

    try:
        vol = cio.read_image(volume_path)
        imageentity.set_volume(vol)
        imageentity.add_child_entity(CellEntity(0, False))
        imageentity.add_child_entity(CellEntity(1, False))
        imageentity.add_child_entity(CellEntity(2, False))
        imageentity.init_default_scenes(vol)
        print("load volume succeed")
        return response(success=True, message='load volume succeed')
    except Exception as err:
        return response(success=False, message='load volume failed')


@command.register('unload')
def unload(**kwargs):
    """
    UnLoad volume data and managed in memory
    :param seriesuid: series uid
    :return:
    """
    try:
        seriesuid = kwargs['seriesuid']
    except Exception as err:
        return response(success=False, message='Invalid parameters.')

    print(seriesuid)
    rst, msg = server.unload_volume(seriesuid)
    print("Unload volume succeed")
    return response(msg)


@command.register('reset')
def reset(**kwargs):
    """
    Reset voulme data
    :return: base64 png image data
    """
    try:
        imageentity.reset_scenes()
        imageentity.updater().update(RefreshType.All)
        result = imageentity.updater().get_result()
        return response(json.dumps(result))
    except Exception as e:
        return response(success=False, message='reset failed')


@command.register('show')
def show(**kwargs):
    """
    Get images
    :return: base64 png image data
    """
    try:
        imageentity.updater().update(RefreshType.All)
        result = imageentity.updater().get_result()
        return response(json.dumps(result))
    except Exception as err:
        return response(success=False, message='show image failed')


@command.register('page')
def page(**kwargs):
    """
    Page turning
    :param delta: cursor delta
    :param focus_view: current focused view, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal
    :return: png base64 image data
    """
    try:
        delta = int(kwargs['delta'])
        focus_view = get_view_index(kwargs['focus_view'])
    except:
        return response(success=False, message='Invalid parameters.')

    imageentity.page(focus_view, delta)
    imageentity.updater().update(RefreshType.All)
    result = imageentity.updater().get_result()
    filter_view = get_page_filter_view(focus_view)
    result = view_filter(result, filter_view)

    return response(json.dumps(result))


@command.register('zoom')
def zoom(**kwargs):
    """
    zoom
    :param kwargs:
    :return:
    """
    try:
        zoom_factor = kwargs['zoom_factor']
        focus_view = get_view_index(kwargs['focus_view'])
    except:
        return response(success=False, message='Invalid parameters.')

    try:
        zoom_factor = float(zoom_factor)
        imageentity.zoom(focus_view, zoom_factor)
        imageentity.updater().update(RefreshType.All)
        result = imageentity.updater().get_result()
        return response(json.dumps(result))
    except Exception as e:
        return response(success=False, message='zoom failed')


@command.register('rotate')
def rotate(**kwargs):
    """
    Rotate
    :param pos_pre:
    :param pos_cur:
    :param focus_view: current focused view, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal
    :return: rgb image data
    """
    try:
        pos_pre = string_int_trans(kwargs['pos_pre'].split(','), 's2i')
        pos_cur = string_int_trans(kwargs['pos_cur'].split(','), 's2i')
        focus_view = get_view_index(kwargs['focus_view'])
        print("=== enter rotate ===")
    except:
        return response(success=False, message='Invalid parameters.')

    try:
        views = imageentity.get_children_views()
        size = views[focus_view].get_scene().get_display_size()
        angle = cal_angle(size, pos_pre, pos_cur)
        imageentity.rotate(focus_view, angle)
        imageentity.updater().update(RefreshType.All)
        result = imageentity.updater().get_result()
        return response(json.dumps(result))
    except Exception as e:
        return response(success=False, message='rotate failed')


@command.register('pan')
def pan(**kwargs):
    """
    Pan
    :param pos_pre:
    :param pos_cur:
    :param focus_view: current focused view, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal
    :return: rgb image data
    """
    try:
        pos_pre = string_int_trans(kwargs['pos_pre'].split(','), 's2i')
        pos_cur = string_int_trans(kwargs['pos_cur'].split(','), 's2i')
        focus_view = get_view_index(kwargs['focus_view'])
    except:
        return response(success=False, message='Invalid parameters.')

    try:
        imageentity.pan(focus_view, pos_pre, pos_cur)
        imageentity.updater().update(RefreshType.All)
        result = imageentity.updater().get_result()
        return response(json.dumps(result))
    except Exception as e:
        return response(success=False, message='pan failed')



@command.register('roll')
def roll(**kwargs):
    """
    Roll
    :param cursor: the 3d world coordinate of a cursor, like [1,1,2], you should pass it like `'1,1,2'`
    :param width: width of viewport
    :param height: height of viewport
    :param focus_view: current focused view, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal
    :param display_view: need to displayed on screen, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal, 'all' for all view
    :return: rgb image data
    """
    try:
        cursor = kwargs['cursor'].split(',')
        cursor = [float(cursor[0]), float(cursor[1]), float(cursor[2])]
        width = int(kwargs['width'])
        height = int(kwargs['height'])
        focus_view = get_view_index(kwargs['focus_view'])
        display_view = get_view_index(kwargs['display_view'])
    except:
        return response(success=False, message='Invalid parameters.')

    server.set_cursor(cursor)
    imgs = server.get_images(display_view, width, height)
    return response(json.dumps(imgs))


@command.register('color')
def color(**kwargs):
    """
    Pseudo-color
    :param colormode: 0 for grayscale, 1 for red, 2 for green, 3 for blue, 4 for jet color, 5 for color config
    :param width: width of viewport
    :param height: height of viewport
    :param focus_view: current focused view, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal
    :param display_view: need to displayed on screen, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal, 'all' for all view
    :return: rgb image data
    """
    try:
        colormode = int(kwargs['colormode'])
        width = int(kwargs['width'])
        height = int(kwargs['height'])
        focus_view = get_view_index(kwargs['focus_view'])
        display_view = get_view_index(kwargs['display_view'])
    except:
        return response(success=False, message='Invalid parameters.')

    server.set_colormode(colormode)
    imgs = server.get_images(display_view, width, height)
    return response(json.dumps(imgs))


@command.register('wcww')
def wcww(**kwargs):
    """
    Change window center and window width
    :param ww_factor:
    :param wl_factor:
    :param focus_view: current focused view, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal
    :return: rgb image data
    """
    try:
        print(kwargs['ww_factor'])
        ww_factor = float(kwargs['ww_factor'])
        wl_factor = float(kwargs['wl_factor'])
        focus_view = get_view_index(kwargs['focus_view'])
    except Exception as err:
        print(err)
        return response(success=False, message='Invalid parameters.')

    try:
        imageentity.window_(focus_view, ww_factor, wl_factor)
        imageentity.updater().update(RefreshType.All)
        result = imageentity.updater().get_result()
        return response(json.dumps(result))
    except Exception as e:
        return response(success=False, message='windows failed')


@command.register('resize')
def resize(**kwargs):
    """
    Resize the view
    :param view_size: the size of front view
    :return:
    """
    try:
        size = kwargs['view_size']
    except:
        return response(success=False, message='Invalid parameters.')

    size = dict(size)
    children_views = imageentity.get_children_views()
    children_views[0].resize_(0, size['transverse'][0], size['transverse'][1])
    children_views[1].resize_(0, size['coronal'][0], size['coronal'][1])
    children_views[2].resize_(0, size['saggital'][0], size['saggital'][1])
    return response(message='Set view size succeed')


@command.register('locate')
def locate(**kwargs):
    try:
        focus_view = get_view_index(kwargs['focus_view'])
        cursor_2D = kwargs['cross_point'].split(",")
    except:
        return response(success=False, message='Invalid parameters.')

    try:
        imageentity.locate(focus_view, cursor_2D)
        imageentity.updater().update(RefreshType.All)
        result = imageentity.updater().get_result()
        result = view_filter(result, focus_view)
        return response(json.dumps(result))
    except Exception as e:
        return response(success=False, message='locate failed')
