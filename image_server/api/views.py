# -*- coding: utf-8 -*-
from __future__ import unicode_literals


from rest_framework.response import Response
from rest_framework.views import APIView

from image_server import ImageServer
from utils import get_display_view, get_focus_view, get_orthogonal_spacing, ViewEnum
import json
from md.image3d.python.image3d import Image3d


server = ImageServer()


class Greeting(APIView):
    """
    Say hi to the world
    """
    def get(self, request):
        return Response('Hi developer, good luck!')


class Load(APIView):
    def post(self, request):
        """
        Load volume data and managed in memory
        :param volume: volume data
        :param seriesuid: series uid
        :return:
        """
        try:
            volume = request.data.get('volume')
            volume = unicode(volume, 'utf-8')
            volume = volume.encode('utf-8')
            volume = json.loads(volume)
            seriesuid = request.data.get('seriesuid')
        except Exception as err:
            return Response('Invalid parameters.', status=500)

        print(seriesuid)
        rst, msg = server.load_volume(volume, seriesuid)
        status = 200 if rst else 500
        return Response(msg, status)


class Reset(APIView):
    def post(self, request):
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
            width = int(request.data.get('width'))
            height = int(request.data.get('height'))
            focus_view = get_focus_view(request.data.get('focus_view'))
            display_view = get_display_view(request.data.get('display_view'))
        except:
            return Response('Invalid parameters.', status=500)

        server.reset_volume()
        imgs = server.get_images(display_view, width, height)
        return Response(json.dumps(imgs))


class Unload(APIView):
    def post(self, request):
        """
        Unload volume data from memory
        :param series_uid: series uid
        :return: Unload result
        """
        seriesuid = request.data.get('seriesuid')
        print(seriesuid)
        # rst, msg = server.load_volume(volume, seriesuid)
        # status = 200 if rst else 500
        # return Response(msg, status)


class Show(APIView):
    def post(self, request):
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
            series_uid = request.data.get('seriesuid')
            width = int(request.data.get('width'))
            height = int(request.data.get('height'))
            focus_view = get_focus_view(request.data.get('focus_view'))
            display_view = get_display_view(request.data.get('display_view'))
        except:
            return Response('Invalid parameters.', status=500)

        vol, cfg = server.change_volume(series_uid)
        if vol is None or cfg is None:
            return Response('Volumn: {} doesn`s exist'.format(series_uid), status=500)

        imgs = server.get_images(display_view, width, height)
        return Response(json.dumps(imgs))


class Page(APIView):
    def post(self, request):
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
            delta = int(request.data.get('delta'))
            width = int(request.data.get('width'))
            height = int(request.data.get('height'))
            focus_view = get_focus_view(request.data.get('focus_view'))
            display_view = get_display_view(request.data.get('display_view'))
        except:
            return Response('Invalid parameters.', status=500)

        server.update_cursor(focus_view, delta)
        imgs = server.get_images(display_view, width, height)
        return Response(json.dumps(imgs))


class Zoom(APIView):
    def post(self, request):
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
            shift = int(request.data.get('shift'))
            width = int(request.data.get('width'))
            height = int(request.data.get('height'))
            focus_view = get_focus_view(request.data.get('focus_view'))
            display_view = get_display_view(request.data.get('display_view'))
        except:
            return Response('Invalid parameters.', status=500)

        server.update_zoom_factor(shift)
        imgs = server.get_images(display_view, width, height)
        return Response(json.dumps(imgs))


class Rotate(APIView):
    def post(self, request):
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
            angle = float(request.data.get('angle'))
            width = int(request.data.get('width'))
            height = int(request.data.get('height'))
            focus_view = get_focus_view(request.data.get('focus_view'))
            display_view = get_display_view(request.data.get('display_view'))
        except:
            return Response('Invalid parameters.', status=500)

        server.update_axis(focus_view, angle)
        imgs = server.get_images(display_view, width, height)
        return Response(json.dumps(imgs))


class Pan(APIView):
    def post(self, request):
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
            shift = request.data.get('shift').split(',')
            shift = [float(shift[0]), float(shift[1]), float(shift[2])]
            width = int(request.data.get('width'))
            height = int(request.data.get('height'))
            focus_view = get_focus_view(request.data.get('focus_view'))
            display_view = get_display_view(request.data.get('display_view'))
        except:
            return Response('Invalid parameters.', status=500)

        server.update_center(shift)
        imgs = server.get_images(display_view, width, height)
        return Response(json.dumps(imgs))


class Roll(APIView):
    def post(self, request):
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
            cursor = request.data.get('cursor').split(',')
            cursor = [float(cursor[0]), float(cursor[1]), float(cursor[2])]
            width = int(request.data.get('width'))
            height = int(request.data.get('height'))
            focus_view = get_focus_view(request.data.get('focus_view'))
            display_view = get_display_view(request.data.get('display_view'))
        except:
            return Response('Invalid parameters.', status=500)

        server.set_cursor(cursor)
        imgs = server.get_images(display_view, width, height)
        return Response(json.dumps(imgs))


class Color(APIView):
    def post(self, request):
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
            colormode = int(request.data.get('colormode'))
            width = int(request.data.get('width'))
            height = int(request.data.get('height'))
            focus_view = get_focus_view(request.data.get('focus_view'))
            display_view = get_display_view(request.data.get('display_view'))
        except:
            return Response('Invalid parameters.', status=500)

        server.set_colormode(colormode)
        imgs = server.get_images(display_view, width, height)
        return Response(json.dumps(imgs))


class WCWW(APIView):
    def post(self, request):
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
            shift = request.data.get('shift').split(',')
            shift = [float(shift[0]), float(shift[1])]
            width = int(request.data.get('width'))
            height = int(request.data.get('height'))
            focus_view = get_focus_view(request.data.get('focus_view'))
            display_view = get_display_view(request.data.get('display_view'))
        except:
            return Response('Invalid parameters.', status=500)

        server.set_wcww(shift)
        imgs = server.get_images(display_view, width, height)
        return Response(json.dumps(imgs))
