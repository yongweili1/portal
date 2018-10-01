# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import base64
import struct

from django.shortcuts import render
import json
# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView
# from patientinformations.models import Series
# from connect_image.user_ip_to_port import ip_port
import ConfigParser
import image_msg_pb2 as msg

from twisted.internet import reactor

from twisted_client import BackEndFactory, be_factory

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

from back_end.util.socket_client import SocketClient, create_app_socket


class Haha(APIView):
    def get(self, request):
        A = request.getgetParameterValues('a')
        B = request.GET.get('b')
        print(A)
        print(B)
        return Response('OK!!!')


class LoadVolume(APIView):

    def get(self, request):
        """
        the client request loads a series
        :param request:a django rest framework request object, include request parameters
        :return:boolean true for success, false for failure
        """
        serid = request.GET.get('seriesuid', None)
        user_ip = request.META.get('REMOTE_ADDR', None)

        volumepath = r'E:\data\SHEN YU-Thorax^10_ZRY_LDCT_Head_first (Adult)-CT.nii.gz'
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

        be_factory.protocol.request(data)

        rst = be_factory.protocol.waiting_for_result()
        return Response(rst.kwargs)

    def delete(self, request):
        """
        the client request unload volume data from memory
        :param request:a django rest framework request object, include request parameters
        :return:boolean true for success, false for failure
        """
        serid = request.GET.get('seriesuid', None)
        user_ip = request.META.get('REMOTE_ADDR', None)
        if serid is None:
            return Response('请输入序列UID')

        dic = {}
        data = {}
        data['seriesuid'] = serid
        dic['session'] = user_ip
        dic['server'] = 'image'
        dic['command'] = "unload"
        dic['context'] = data
        sock = SocketClient()
        resp = sock.upload_volume(str(dic))

        # try:
        #     resp = sock.request(str(dic))
        # except Exception as e:
        #     return Response('image_server Error')

        resp = json.loads(resp)
        if not resp['success']:
            return Response(resp['msg'])

        return Response(resp['content'])


class GetImage(APIView):

    def get(self, request):
        """
        show images
        :param request: a django rest framework request object, include request parameters
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

        data = msg.RequestMsg()
        data.session = '10.9.19.139'
        data.server_name = 'image'
        data.command = 'show'
        data.content.params = json.dumps({'seriesuid': serid,
                                          'width': 400,
                                          'height': 400,
                                          'focus_view': '',
                                          'display_view': 'all'})
        data = data.SerializeToString()
        size = len(data)
        size = struct.pack('i', size)
        data = size + data

        be_factory.protocol.request(data)
        rst = be_factory.protocol.waiting_for_result()
        return Response(rst.kwargs)


class Home(APIView):

    def get(self, request):
        user_ip = request.META.get('REMOTE_ADDR', None)

        return render(request, 'main.html')


class GetSeriesUidList(APIView):
    def get(self, request):
        # seriesuidlist = []
        # serieslist = Series.objects.all()
        # for ser in serieslist:
        #     seriesuidlist.append(ser.seriesuid)
        return Response(None)


class ChangeColor(APIView):

    def get(self, request):
        """
        Change the color of the image
        :param request:a django rest framework request object, include request parameters
        :return:rgb image data
        """
        colormode = request.GET.get('colormode', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)
        if width is None or height is None:
            return Response('请输入完整的请求数据')

        dic = {}
        data = {}
        data['colormode'] = colormode
        data['width'] = width
        data['height'] = height
        data['focus_view'] = focus_view
        data['display_view'] = display_view
        dic['session'] = user_ip
        dic['server'] = 'image'
        dic['command'] = "color"
        dic['context'] = data
        sock = SocketClient()
        resp = sock.upload_volume(str(dic))

        # try:
        #     resp = sock.request(str(dic))
        # except Exception as e:
        #     return Response('image_server Error')

        resp = json.loads(resp)
        if not resp['success']:
            return Response(resp['msg'])

        return Response(resp['content'])


class TurnPage(APIView):

    def get(self, request):
        """
        Page turning
        :param request:a django rest framework request object, include request parameters
        :return:rgb image data
        """
        delta = request.GET.get('delta', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)
        if width is None or height is None:
            return Response('请输入完整的请求数据')


        dic = {}
        data = {}
        data['delta'] = delta
        data['width'] = width
        data['height'] = height
        data['focus_view'] = focus_view
        data['display_view'] = display_view
        dic['session'] = user_ip
        dic['server'] = 'image'
        dic['command'] = "page"
        dic['context'] = data
        sock = SocketClient()
        resp = sock.upload_volume(str(dic))

        # try:
        #     resp = sock.request(str(dic))
        # except Exception as e:
        #     return Response('image_server Error')

        resp = json.loads(resp)
        if not resp['success']:
            return Response(resp['msg'])

        return Response(resp['content'])


class Pan(APIView):

    def get(self, request):
        """
        translation
        :param request:a django rest framework request object, include request parameters
        :return:rgb image data
        """
        shift = request.GET.get('shift', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)
        if width is None or height is None:
            return Response('请输入完整的请求数据')

        dic = {}
        data = {}
        data['shift'] = shift
        data['width'] = width
        data['height'] = height
        data['focus_view'] = focus_view
        data['display_view'] = display_view
        dic['session'] = user_ip
        dic['server'] = 'image'
        dic['command'] = "pan"
        dic['context'] = data
        sock = SocketClient()
        resp = sock.upload_volume(str(dic))

        # try:
        #     resp = sock.request(str(dic))
        # except Exception as e:
        #     return Response('image_server Error')

        resp = json.loads(resp)
        if not resp['success']:
            return Response(resp['msg'])

        return Response(resp['content'])


class Roll(APIView):

    def get(self, request):
        """
        roll
        :param request:a django rest framework request object, include request parameters
        :return:rgb image data
        """
        cursor = request.GET.get('cursor', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)
        if width is None or height is None:
            return Response('请输入完整的请求数据')


        # dic = {}
        # data = {}
        # data['cursor'] = cursor
        # data['width'] = width
        # data['height'] = height
        # data['focus_view'] = focus_view
        # data['display_view'] = display_view
        # dic['session'] = user_ip
        # dic['server'] = 'image'
        # dic['command'] = "roll"
        # dic['context'] = data

        msg_one = {}
        msg_one['flag'] = 'start'
        msg_one['size'] = ''
        msg_one['content'] = 'start'
        msg_two = {}
        msg_one['flag'] = 'data'
        msg_one['size'] = ''
        msg_one['content'] = 'data'
        msg_three = {}
        msg_three['flag'] = 'finish'
        msg_three['size'] = ''
        msg_three['content'] = 'finish'


        sock = SocketClient()
        resp = sock.upload_volume(str(msg_one), str(msg_two), str(msg_three))

        # try:
        #     resp = sock.request(str(dic))
        # except Exception as e:
        #     return Response('image_server Error')

        resp = json.loads(resp)
        if not resp['success']:
            return Response(resp['msg'])

        return Response(resp['content'])


class Rotate(APIView):

    def get(self, request):
        """
        rotate
        :param request:a django rest framework request object, include request parameters
        :return:rgb image data
        """
        angle = request.GET.get('angle', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)
        if width is None or height is None:
            return Response('请输入完整的请求数据')

        dic = {}
        data = {}
        data['angle'] = angle
        data['width'] = width
        data['height'] = height
        data['focus_view'] = focus_view
        data['display_view'] = display_view
        dic['session'] = user_ip
        dic['server'] = 'image'
        dic['command'] = "rotate"
        dic['context'] = data
        sock = SocketClient()
        resp = sock.upload_volume(str(dic))

        # try:
        #     resp = sock.request(str(dic))
        # except Exception as e:
        #     return Response('image_server Error')

        resp = json.loads(resp)
        if not resp['success']:
            return Response(resp['msg'])

        return Response(resp['content'])


class Zoom(APIView):

    def get(self, request):
        """
        zoom
        :param request:a django rest framework request object, include request parameters
        :return:rgb image data
        """
        shift = request.GET.get('shift', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)
        if width is None or height is None:
            return Response('请输入完整的请求数据')


        dic = {}
        data = {}
        data['shift'] = shift
        data['width'] = width
        data['height'] = height
        data['focus_view'] = focus_view
        data['display_view'] = display_view
        dic['session'] = user_ip
        dic['server'] = 'image'
        dic['command'] = "zoom"
        dic['context'] = data
        sock = SocketClient()
        resp = sock.upload_volume(str(dic))

        # try:
        #     resp = sock.request(str(dic))
        # except Exception as e:
        #     return Response('image_server Error')

        resp = json.loads(resp)
        if not resp['success']:
            return Response(resp['msg'])

        return Response(resp['content'])


class ReSetVolume(APIView):

    def get(self, request):
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)
        if width is None or height is None:
            return Response('请输入完整的请求数据')

        dic = {}
        data = {}
        data['width'] = width
        data['height'] = height
        data['focus_view'] = focus_view
        data['display_view'] = display_view
        dic['session'] = user_ip
        dic['server'] = 'image'
        dic['command'] = "reset"
        dic['context'] = data
        sock = SocketClient()
        resp = sock.upload_volume(str(dic))

        # try:
        #     resp = sock.request(str(dic))
        # except Exception as e:
        #     return Response('image_server Error')

        resp = json.loads(resp)
        if not resp['success']:
            return Response(resp['msg'])

        return Response(resp['content'])


class ChangeWindow(APIView):

    def get(self, request):
        shift = request.GET.get('shift', '0')
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)
        if width is None or height is None:
            return Response('请输入完整的请求数据')

        dic = {}
        data = {}
        data['shift'] = shift
        data['width'] = width
        data['height'] = height
        data['focus_view'] = focus_view
        data['display_view'] = display_view
        dic['session'] = user_ip
        dic['server'] = 'image'
        dic['command'] = "wcww"
        dic['context'] = data
        sock = SocketClient()
        resp = sock.upload_volume(str(dic))

        # try:
        #     resp = sock.request(str(dic))
        # except Exception as e:
        #     return Response('image_server Error')

        resp = json.loads(resp)
        if not resp['success']:
            return Response(resp['msg'])

        return Response(resp['content'])


class SayHi(APIView):
    def get(self, request):
        # user_ip = request.META.get('REMOTE_ADDR', None)
        # dic = {}
        # dic['session'] = user_ip
        # dic['command'] = "aaaaaa"
        # message = json.dumps(dic)
        # sock = SocketClient('10.9.19.148', 8888)
        # resp = sock.request(message)
        resp = Response('PK', headers={"Access-Control-Allow-Methods": "GET,PUT,DELETE,POST,OPTIONS", "Access-Control-Allow-Headers":"Content-Type"})
        return resp
