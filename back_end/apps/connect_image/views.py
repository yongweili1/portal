# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import struct

from django.shortcuts import render
import json
# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView
# from connect_image.user_ip_to_port import ip_port
import ConfigParser
import image_msg_pb2 as msg
from patientinformations.models import Series

from twisted_client import be_factory
from connect_image.macro_recording import Macro


conf = ConfigParser.ConfigParser()
conf.read('back_end/util/serverApi.ini')
load_url = conf.get("imageApi", "load_url")
provide_image_url = conf.get("imageApi", "provide_image_url")
change_color_url = conf.get("imageApi", "change_color_url")
turn_page_url = conf.get("imageApi", "turn_page_url")
pan_url = conf.get("imageApi", "pan_url")
roll_url = conf.get("imageApi", "roll_url")
rotate_url = conf.get("imageApi", "rotate_url")
zoom_url = conf.get("imageApi", "zoom_url")
unload_url = conf.get("imageApi", "unload_url")
reset_url = conf.get("imageApi", "reset_url")
change_window_url = conf.get("imageApi", "change_window_url")


@Macro()
def visit_image_server(data):

    be_factory.protocol.request(data)
    rst = be_factory.protocol.waiting_for_result()

    return rst


class MacroRecording(APIView):

    def get(self, request):
        macro_status = request.GET.get('macro_status', None)
        Macro.macro_status = macro_status
        # if macro_status is True:

        return Response()
# class LoadVolume(APIView):
#
#     # @recordMacro(macroRecord)
#     # def aaa(self):
#     #     pass
#     def get(self, request):
#
#         # macroRecord = True
#         # aaa()
#
#         """
#         the client request loads a series
#         :param seriesuid: series uid
#         :return:boolean true for success, false for failure
#         """
#         serid = request.GET.get('seriesuid', None)
#         user_ip = request.META.get('REMOTE_ADDR', None)
#
#         series_query = Series.objects.filter(seriesuid=serid)
#         if len(series_query) == 0:
#             return Response('数据库无此seriesuid')
#         volumepath = series_query[0].seriespixeldatafilepath
#         f = open(volumepath, 'rb')
#         vol = f.read()
#         f.close()
#
#         data = msg.RequestMsg()
#         data.session = user_ip
#         data.server_name = 'image'
#         data.command = 'load'
#         data.content.params = json.dumps({'seriesuid': serid})
#         data.content.volume = vol
#         data = data.SerializeToString()
#         size = len(data)
#         size = struct.pack('i', size)
#         data = size + data
#
#         be_factory.protocol.request(data)
#
#         serid = request.GET.get('seriesuid', None)
#         width = request.GET.get('width', None)
#         height = request.GET.get('height', None)
#         focus_view = request.GET.get('focus_view', None)
#         display_view = request.GET.get('display_view', 'transverse')
#         user_ip = request.META.get('REMOTE_ADDR', None)
#
#         if width is None or height is None:
#             return Response('请输入完整的请求数据')
#
#         data = msg.RequestMsg()
#         data.session = user_ip
#         data.server_name = 'image'
#         data.command = 'show'
#         data.content.params = json.dumps({'seriesuid': serid,
#                                           'width': width,
#                                           'height': height,
#                                           'focus_view': focus_view,
#                                           'display_view': display_view
#                                           })
#         data = data.SerializeToString()
#         size = len(data)
#         size = struct.pack('i', size)
#         data = size + data
#
#         try:
#             rst = visit_image_server(data)
#         except:
#             return Response('服务间数据传输失败')
#
#         return Response(rst.kwargs)
#
#     def delete(self, request):
#         """
#         the client request unload volume data from memory
#         :param seriesuid: series uid
#         :return:boolean true for success, false for failure
#         """
#         serid = request.GET.get('seriesuid', None)
#         user_ip = request.META.get('REMOTE_ADDR', None)
#
#         if serid is None:
#             return Response('请输入序列UID')
#
#         data = msg.RequestMsg()
#         data.session = user_ip
#         data.server_name = 'image'
#         data.command = 'unload'
#         data.content.params = json.dumps({'seriesuid': serid})
#         data = data.SerializeToString()
#         size = len(data)
#         size = struct.pack('i', size)
#         data = size + data
#
#         try:
#             rst = visit_image_server(data)
#         except:
#             return Response('服务间数据传输失败')
#
#         return Response(rst.kwargs)


class GetImage(APIView):

    def get(self, request):
        """
        the client request loads a series and get images
        :param seriesuid: series uid
        :param width: width of viewport
        :param height: height of viewport
        :param focus_view: current focused view, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal
        :param display_view: need to displayed on screen, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal, 'all' for all view
        :return: rgb image data
        """
        serid = request.GET.get('seriesuid', None)
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)

        if width is None or height is None:
            return Response('请输入完整的请求数据')

        series_query = Series.objects.filter(seriesuid=serid)
        if len(series_query) == 0:
            return Response('数据库无此seriesuid')

        volumepath = series_query[0].seriespixeldatafilepath
        f = open(volumepath, 'rb')
        vol = f.read()
        f.close()

        data = msg.RequestMsg()
        data.session = user_ip
        data.server_name = 'image'
        data.command = 'load'
        data.content.params = json.dumps({'seriesuid': serid})
        data.content.volume = vol
        data = data.SerializeToString()
        size = len(data)
        size = struct.pack('i', size)
        data = size + data

        try:
            rst = visit_image_server(data)
        except:
            return Response('服务间数据传输失败')

        data = msg.RequestMsg()
        data.session = user_ip
        data.server_name = 'image'
        data.command = 'show'
        data.content.params = json.dumps({'seriesuid': serid,
                                          'width': width,
                                          'height': height,
                                          'focus_view': focus_view,
                                          'display_view': display_view
                                          })
        data = data.SerializeToString()
        size = len(data)
        size = struct.pack('i', size)
        data = size + data

        try:
            rst = visit_image_server(data)
        except:
            return Response('服务间数据传输失败')

        return Response(rst.kwargs)


class Home(APIView):

    def get(self, request):
        # user_ip = request.META.get('REMOTE_ADDR', None)

        return render(request, 'main.html')


class GetSeriesUidList(APIView):
    def get(self, request):
        """
        provide a list of seriesuid
        :param request:
        :return:a list of seriesuid
        """
        seriesuidlist = []
        serieslist = Series.objects.all()
        for ser in serieslist:
            seriesuidlist.append(ser.seriesuid)
        return Response(seriesuidlist)


class ChangeColor(APIView):

    def get(self, request):
        """
        Change the color of the image
        :param colormode: 0 for grayscale, 1 for red, 2 for green, 3 for blue, 4 for jet color, 5 for color config
        :param width: width of viewport
        :param height: height of viewport
        :param focus_view: current focused view, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal
        :param display_view: need to displayed on screen, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal, 'all' for all view
        :return: rgb image data
        """
        colormode = request.GET.get('colormode', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)

        if width is None or height is None:
            return Response('请输入完整的请求数据')

        data = msg.RequestMsg()
        data.session = user_ip
        data.server_name = 'image'
        data.command = 'color'
        data.content.params = json.dumps({'colormode': colormode,
                                          'width': width,
                                          'height': height,
                                          'focus_view': focus_view,
                                          'display_view': display_view
                                          })
        data = data.SerializeToString()
        size = len(data)
        size = struct.pack('i', size)
        data = size + data

        try:
            rst = visit_image_server(data)
        except:
            return Response('服务间数据传输失败')

        return Response(rst.kwargs)


class TurnPage(APIView):

    def get(self, request):
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
        delta = request.GET.get('delta', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)

        if width is None or height is None:
            return Response('请输入完整的请求数据')

        data = msg.RequestMsg()
        data.session = user_ip
        data.server_name = 'image'
        data.command = 'page'
        data.content.params = json.dumps({'delta': delta,
                                          'width': width,
                                          'height': height,
                                          'focus_view': focus_view,
                                          'display_view': display_view
                                          })
        data = data.SerializeToString()
        size = len(data)
        size = struct.pack('i', size)
        data = size + data

        try:
            rst = visit_image_server(data)
        except:
            return Response('服务间数据传输失败')

        return Response(rst.kwargs)


class Pan(APIView):

    def get(self, request):
        """
        translation
        :param shift: center shift, like [1,1,2], you should pass it like `'1,1,2'`
        :param width: width of viewport
        :param height: height of viewport
        :param focus_view: current focused view, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal
        :param display_view: need to displayed on screen, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal, 'all' for all view
        :return: rgb image data
        """
        shift = request.GET.get('shift', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)

        if width is None or height is None:
            return Response('请输入完整的请求数据')

        data = msg.RequestMsg()
        data.session = user_ip
        data.server_name = 'image'
        data.command = 'pan'
        data.content.params = json.dumps({'shift': shift,
                                          'width': width,
                                          'height': height,
                                          'focus_view': focus_view,
                                          'display_view': display_view
                                          })
        data = data.SerializeToString()
        size = len(data)
        size = struct.pack('i', size)
        data = size + data

        try:
            rst = visit_image_server(data)
        except:
            return Response('服务间数据传输失败')

        return Response(rst.kwargs)


class Roll(APIView):

    def get(self, request):
        """
        roll
        :param cursor: the 3d world coordinate of a cursor, like [1,1,2], you should pass it like `'1,1,2'`
        :param width: width of viewport
        :param height: height of viewport
        :param focus_view: current focused view, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal
        :param display_view: need to displayed on screen, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal, 'all' for all view
        :return: rgb image data
        """
        cursor = request.GET.get('cursor', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)

        if width is None or height is None:
            return Response('请输入完整的请求数据')

        data = msg.RequestMsg()
        data.session = user_ip
        data.server_name = 'image'
        data.command = 'roll'
        data.content.params = json.dumps({'cursor': cursor,
                                          'width': width,
                                          'height': height,
                                          'focus_view': focus_view,
                                          'display_view': display_view
                                          })
        data = data.SerializeToString()
        size = len(data)
        size = struct.pack('i', size)
        data = size + data

        try:
            rst = visit_image_server(data)
        except:
            return Response('服务间数据传输失败')

        return Response(rst.kwargs)


class Rotate(APIView):

    def get(self, request):
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
        angle = request.GET.get('angle', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)

        if width is None or height is None:
            return Response('请输入完整的请求数据')

        data = msg.RequestMsg()
        data.session = user_ip
        data.server_name = 'image'
        data.command = 'rotate'
        data.content.params = json.dumps({'angle': angle,
                                          'width': width,
                                          'height': height,
                                          'focus_view': focus_view,
                                          'display_view': display_view
                                          })
        data = data.SerializeToString()
        size = len(data)
        size = struct.pack('i', size)
        data = size + data

        try:
            rst = visit_image_server(data)
        except:
            return Response('服务间数据传输失败')

        return Response(rst.kwargs)


class Zoom(APIView):

    def get(self, request):
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
        shift = request.GET.get('shift', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)

        if width is None or height is None:
            return Response('请输入完整的请求数据')

        data = msg.RequestMsg()
        data.session = user_ip
        data.server_name = 'image'
        data.command = 'zoom'
        data.content.params = json.dumps({'shift': shift,
                                          'width': width,
                                          'height': height,
                                          'focus_view': focus_view,
                                          'display_view': display_view
                                          })
        data = data.SerializeToString()
        size = len(data)
        size = struct.pack('i', size)
        data = size + data

        try:
            rst = visit_image_server(data)
        except:
            return Response('服务间数据传输失败')

        return Response(rst.kwargs)


class ReSetVolume(APIView):

    def get(self, request):
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
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)

        if width is None or height is None:
            return Response('请输入完整的请求数据')

        data = msg.RequestMsg()
        data.session = user_ip
        data.server_name = 'image'
        data.command = 'reset'
        data.content.params = json.dumps({'width': width,
                                          'height': height,
                                          'focus_view': focus_view,
                                          'display_view': display_view
                                          })
        data = data.SerializeToString()
        size = len(data)
        size = struct.pack('i', size)
        data = size + data

        try:
            rst = visit_image_server(data)
        except:
            return Response('服务间数据传输失败')

        return Response(rst.kwargs)


class ChangeWindow(APIView):

    def get(self, request):
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
        shift = request.GET.get('shift', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)

        if width is None or height is None:
            return Response('请输入完整的请求数据')

        data = msg.RequestMsg()
        data.session = user_ip
        data.server_name = 'image'
        data.command = 'wcww'
        data.content.params = json.dumps({'shift': shift,
                                          'width': width,
                                          'height': height,
                                          'focus_view': focus_view,
                                          'display_view': display_view
                                          })
        data = data.SerializeToString()
        size = len(data)
        size = struct.pack('i', size)
        data = size + data

        try:
            rst = visit_image_server(data)
        except:
            return Response('服务间数据传输失败')

        return Response(rst.kwargs)
