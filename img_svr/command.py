# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from image_server import ImageServer
from message import response
from utilities import get_display_view, get_focus_view, get_orthogonal_spacing, ViewEnum

import json


class Command(object):
    def __init__(self):
        self.commands = {}

    def register(self, command):
        print('Register command {} succeed.'.format(command))
        def wrapper(func):
            self.commands[command] = func
            return func
        return wrapper


# single instance of Command
command = Command()


# single instance of ImageServer
server = ImageServer()


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

    print(seriesuid)
    rst, msg = server.load_volume(volume_path, seriesuid)
    return response(msg)


@command.register('reset')
def reset(**kwargs):
    """
    Reset voulme data
    :param width: width of viewport
    :param height: height of viewport
    :param focus_view: current focused view, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal
    :param display_view: need to displayed on screen, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal, 'all' for all view
    :return: True or False
    """
    try:
        width = int(kwargs['width'])
        height = int(kwargs['height'])
        focus_view = get_focus_view(kwargs['focus_view'])
        display_view = get_display_view(kwargs['display_view'])
    except:
        return response(success=False, message='Invalid parameters.')

    server.reset_volume()
    imgs = server.get_images(display_view, width, height)
    return response(json.dumps(imgs))


@command.register('unload')
def unload(**kwargs):
    """
    Unload volume data from memory
    :param series_uid: series uid
    :return: Unload result
    """
    seriesuid = kwargs['seriesuid']
    print(seriesuid)
    # rst, msg = server.load_volume(volume, seriesuid)
    # status = 200 if rst else 500
    # return ResponseData(msg, status)


@command.register('show')
def show(**kwargs):
    """
    Get images
    :param series_uid: series uid
    :param width: width of viewport
    :param height: height of viewport
    :param focus_view: current focused view, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal
    :param display_view: need to displayed on screen, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal, 'all' for all view
    :return: rgb image data
    """
    try:
        series_uid = kwargs['seriesuid']
        width = int(kwargs['width'])
        height = int(kwargs['height'])
        # focus_view = get_focus_view(kwargs['focus_view'])
        display_view = get_display_view(kwargs['display_view'])
    except:
        return response(success=False, message='Invalid parameters.')

    vol, cfg = server.change_volume(series_uid)
    if vol is None or cfg is None:
        return response(success=False,
                            message='Volumn: {} doesn`s exist'.format(series_uid))

    imgs = server.get_images(display_view, width, height)
    return response(json.dumps(imgs), message=kwargs['display_view'])


@command.register('page')
def page(**kwargs):
    """
    Page turning
    :param delta: cursor delta
    :param width: width of viewport
    :param height: height of viewport
    :param focus_view: current focused view, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal
    :param display_view: need to displayed on screen, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal, 'all' for all view
    :return: rgb image data
    """
    try:
        delta = int(kwargs['delta'])
        width = int(kwargs['width'])
        height = int(kwargs['height'])
        focus_view = get_focus_view(kwargs['focus_view'])
        display_view = get_display_view(kwargs['focus_view'])
    except:
        return response(success=False, message='Invalid parameters.')

    data = {}
    cursor_3d = server.update_cursor(focus_view, delta)
    server.update_look_at(focus_view, delta)
    imgs = server.get_images(display_view, width, height)
    trans_para = {
        'point_3d': cursor_3d,
        'trans_direct_flag': 'world2screen',
    }
    cursor_2d = server.dimension_translate(trans_para)
    data['cross_position'] = cursor_2d
    data.update(imgs)
    return response(json.dumps(data))


@command.register('zoom')
def zoom(**kwargs):
    """
    Zoom
    :param shift: zoom factor shift
    :param width: width of viewport
    :param height: height of viewport
    :param focus_view: current focused view, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal
    :param display_view: need to displayed on screen, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal, 'all' for all view
    :return: rgb image data
    """
    try:
        shift = int(kwargs['shift'])
        width = int(kwargs['width'])
        height = int(kwargs['height'])
        focus_view = get_focus_view(kwargs['focus_view'])
        display_view = get_display_view(kwargs['display_view'])
    except:
        return response(success=False, message='Invalid parameters.')

    server.update_zoom_factor(shift)
    imgs = server.get_images(display_view, width, height)
    return response(json.dumps(imgs))


@command.register('rotate')
def rotate(**kwargs):
    """
    Rotate
    :param angle: rotate angle
    :param width: width of viewport
    :param height: height of viewport
    :param focus_view: current focused view, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal
    :param display_view: need to displayed on screen, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal, 'all' for all view
    :return: rgb image data
    """
    try:
        angle = float(kwargs['angle'])
        width = int(kwargs['width'])
        height = int(kwargs['height'])
        focus_view = get_focus_view(kwargs['focus_view'])
        display_view = get_display_view(kwargs['display_view'])
    except:
        return response(success=False, message='Invalid parameters.')

    server.update_axis(focus_view, angle)
    imgs = server.get_images(display_view, width, height)
    return response(json.dumps(imgs))


@command.register('pan')
def pan(**kwargs):
    """
    Pan
    :param shift: center shift, like [1,1,2], you should pass it like `'1,1,2'`
    :param width: width of viewport
    :param height: height of viewport
    :param focus_view: current focused view, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal
    :param display_view: need to displayed on screen, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal, 'all' for all view
    :return: rgb image data
    """
    try:
        shift = kwargs['shift'].split(',')
        shift = [float(shift[0]), float(shift[1]), float(shift[2])]
        width = int(kwargs['width'])
        height = int(kwargs['height'])
        focus_view = get_focus_view(kwargs['focus_view'])
        display_view = get_display_view(kwargs['display_view'])
    except:
        return response(success=False, message='Invalid parameters.')

    server.update_center(shift)
    imgs = server.get_images(display_view, width, height)
    return response(json.dumps(imgs))


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
        focus_view = get_focus_view(kwargs['focus_view'])
        display_view = get_display_view(kwargs['display_view'])
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
        focus_view = get_focus_view(kwargs['focus_view'])
        display_view = get_display_view(kwargs['display_view'])
    except:
        return response(success=False, message='Invalid parameters.')

    server.set_colormode(colormode)
    imgs = server.get_images(display_view, width, height)
    return response(json.dumps(imgs))


@command.register('wcww')
def wcww(**kwargs):
    """
    Change window center and window width
    :param shift: shift of mouse point, like [10, 10], you should pass it like `'10, 10'`
    :param width: width of viewport
    :param height: height of viewport
    :param focus_view: current focused view, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal
    :param display_view: need to displayed on screen, 'transverse' for transverse,
    'saggital' for saggital, 'coronal' for coronal, 'all' for all view
    :return: rgb image data
    """
    try:
        shift = kwargs['shift'].split(',')
        shift = [float(shift[0]), float(shift[1])]
        width = int(kwargs['width'])
        height = int(kwargs['height'])
        focus_view = get_focus_view(kwargs['focus_view'])
        display_view = get_display_view(kwargs['display_view'])
    except:
        return response(success=False, message='Invalid parameters.')

    server.set_wcww(shift)
    imgs = server.get_images(display_view, width, height)
    return response(json.dumps(imgs))


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
    server.set_view_size(**size)
    return response(message='Setting view size succeed')


@command.register('locate')
def locate(**kwargs):
    """
    Resize the view
    :param view_size: the size of front view
    :return:
    """
    try:
        focus_view = kwargs['focus_view']
        cursor_2D = kwargs['cursor_2D']
        display_view = kwargs['display_view']
    except:
        return response(success=False, message='Invalid parameters.')

    trans_para = {
        'point_2d': cursor_2D,
        'trans_direct_flag': 'screen2world',
        'focus_view': focus_view
    }
    display_view_array = display_view.split(",")
    cursor_3d = server.dimension_translate(trans_para)
    server.set_cursor(cursor_3d)
    server.set_look_at(cursor_3d, cursor_3d, cursor_3d)
    imgs = server.get_multi_images(display_view_array)
    trans_para = {
        'point_3d': cursor_3d,
        'trans_direct_flag': 'world2screen'
    }
    data = {}
    cursor_2d = server.dimension_translate(trans_para)
    data['cross_position'] = cursor_2d
    data.update(imgs)
    return response(json.dumps(data))
