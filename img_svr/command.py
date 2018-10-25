# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import time

from entity.cellentity import CellEntity
from image_server import ImageServer
from message import response
from updater.args import RefreshType
from utilities import get_display_view, get_focus_view, get_orthogonal_spacing, ViewEnum, get_view_index
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

    # vol, cfg = server.change_volume(series_uid)
    # if vol is None or cfg is None:
    #     return response(success=False,
    #                         message='Volumn: {} doesn`s exist'.format(series_uid))
    #
    # imgs = server.get_images(display_view, width, height)
    imageentity.updater().update(RefreshType.All)
    result = imageentity.updater().get_result()
    return response(json.dumps(result))


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

    view_index = get_view_index(focus_view)
    imageentity.page(view_index, delta)
    imageentity.updater().update(RefreshType.All)
    result = imageentity.updater().get_result()
    return response(json.dumps(result))

    # a = time.time()
    # data = {}
    # cursor_3d = server.update_cursor(focus_view, delta)
    # server.update_look_at(focus_view, delta)
    # imgs = server.get_images(display_view, width, height)
    # trans_para = {
    #     'point_3d': cursor_3d,
    #     'trans_direct_flag': 'world2screen',
    # }
    # cursor_2d = server.dimension_translate(trans_para)
    # data['cross_position'] = cursor_2d
    # data.update(imgs)
    # b = time.time()
    # print("svr page cost time  = %f ms" % ((b-a)*1000))
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
    children_views = imageentity.get_children_views()
    children_views[0].resize_(0, size['transverse'][0], size['transverse'][1])
    children_views[0].resize_(0, size['coronal'][0], size['coronal'][1])
    children_views[0].resize_(0, size['saggital'][0], size['saggital'][1])
    print("resize succeed")
    return response(message='Set view size succeed')


@command.register('locate')
def locate(**kwargs):
    try:
        focus_view = kwargs['focus_view']
        cursor_2D = kwargs['cross_point'].split(",")
        display_view = kwargs['display_view']
    except:
        return response(success=False, message='Invalid parameters.')

    view_index = get_view_index(focus_view)
    imageentity.locate(view_index, cursor_2D)
    imageentity.updater().update(RefreshType.All)
    result = imageentity.updater().get_result()
    return response(json.dumps(result))

    # a = time.time()
    # trans_para = {
    #     'point_2d': cursor_2D,
    #     'trans_direct_flag': 'screen2world',
    #     'focus_view': focus_view
    # }
    # cursor_3d = server.dimension_translate(trans_para)
    # server.set_cursor(cursor_3d)
    # trans_para = {
    #     'point_3d': cursor_3d,
    #     'trans_direct_flag': 'world2screen'
    # }
    # cursor_2d = server.dimension_translate(trans_para)
    # server.set_look_at(cursor_3d, cursor_3d, cursor_3d)
    # display_view_array = display_view.split(",")
    # imgs = server.get_multi_images(display_view_array)
    # data = {}
    # data['cross_position'] = cursor_2d
    # data.update(imgs)
    # b = time.time()
    # print("svr cost time  = %d" % (b-a))
    # print(data['cross_position'])
    # return response(json.dumps(data))
