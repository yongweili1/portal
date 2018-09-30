# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import base64

from django.shortcuts import render
import json
# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView
from patientinformations.models import Series
from connect_image.user_ip_to_port import ip_port
import ConfigParser

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

from back_end.util.socket_client import SocketClient, create_socket, create_app_socket


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
        # if serid is None:
        #     return Response('请输入序列UID')
        #
        # seriesobject = Series.objects.filter(seriesuid=serid)
        # if len(seriesobject) == 0:
        #     return Response('请输入正确的序列ID')

        # volumepath = seriesobject[0].seriespixeldatafilepath

        # f = open(volumepath, 'rb')
        # vol = f.read()
        # f.close()
        #
        # data = {}
        # data['command'] = 'load'
        # data['session'] = user_ip
        # data['server'] = 'image'
        # data['content'] = {
        #     'seriesuid': serid,
        #     'volume': vol
        # }
        #
        # msg_one = {}
        # msg_one['flag'] = 'start'
        # msg_one['size'] = 0
        # msg_one['content'] = 'start'
        # msg_two = {}
        # msg_two['flag'] = 'data'
        # msg_two['size'] = 0
        # msg_two['content'] = vol
        # msg_three = {}
        # msg_three['flag'] = 'finish'
        # msg_three['size'] = 0
        # msg_three['content'] = 'finish'
        #
        # sock = create_app_socket(ip_port[user_ip])
        # print(msg_one)
        # sock.sendall(str(msg_one))
        # print(sock.recv(1024))
        # sock.close()
        #
        # sock1 = create_app_socket(ip_port[user_ip])
        # sock1.sendall(vol)
        # rst = sock1.recv(1024)
        # sock1.close()
        #
        # sock2 = create_app_socket(ip_port[user_ip])
        # print(msg_three)
        # sock2.sendall(str(msg_three))
        # print(sock2.recv(1024))
        # sock2.close()

        with open('C:\\Users\\daoming.wang\\Desktop\\timg.png', 'rb') as f:
            data = f.read()
        data = base64.b64encode(data)
        head_str = 'data:image/png;base64,'
        data = head_str + data

        return Response(data)

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

        data = {}
        data['command'] = 'load'
        data['session'] = user_ip
        data['server'] = 'image'
        data['content'] = {
            'seriesuid': serid,
            'width': width,
            'height': height,
            'focus_view': focus_view,
            'display_view': display_view
        }

        msg_one = {}
        msg_one['flag'] = 'start'
        msg_one['size'] = 0
        msg_one['content'] = 'start'
        msg_two = {}
        msg_two['flag'] = 'data'
        msg_two['size'] = 0
        msg_two['content'] = str(data)
        msg_three = {}
        msg_three['flag'] = 'finish'
        msg_three['size'] = 0
        msg_three['content'] = 'finish'

        sock = create_app_socket(ip_port[user_ip])
        print(msg_one)
        sock.sendall(str(msg_one))
        print(sock.recv(1024))
        sock.close()

        sock1 = create_app_socket(ip_port[user_ip])
        print(msg_two)
        sock1.sendall(str(msg_two))
        rst = sock1.recv(1024)
        sock1.close()

        sock2 = create_app_socket(ip_port[user_ip])
        print(msg_three)
        sock2.sendall(str(msg_three))
        print(sock2.recv(1024))
        sock2.close()

        return Response(json.loads(rst))


class Home(APIView):

    def get(self, request):
        user_ip = request.META.get('REMOTE_ADDR', None)

        msg_one = {}
        msg_one['flag'] = 'start'
        msg_one['size'] = 0
        msg_one['content'] = 'start'
        msg_two = {}
        msg_two['flag'] = 'data'
        msg_two['size'] = 0
        msg_two['content'] = '{},register'.format(user_ip)
        msg_three = {}
        msg_three['flag'] = 'finish'
        msg_three['size'] = 0
        msg_three['content'] = 'finish'

        sock = create_socket()
        print(msg_one)
        sock.sendall(str(msg_one))
        print(sock.recv(1024))
        sock.close()

        sock1 = create_socket()
        print(msg_two)
        sock1.sendall(str(msg_two))
        print(sock1.recv(1024))
        sock1.close()

        sock2 = create_socket()
        print(msg_three)
        sock2.sendall(str(msg_three))
        port = sock2.recv(1024)
        ip_port[user_ip] = int(port)
        sock2.close()

        # sock = SocketClient()
        # resp = sock.upload_volume(str(dic))
        #
        # try:
        #     resp = sock.upload_volume(str(dic))
        # except Exception as e:
        #     pass
        #     # return Response('image_server Error')

        return render(request, 'main.html')


class GetSeriesUidList(APIView):
    def get(self, request):
        seriesuidlist = []
        serieslist = Series.objects.all()
        for ser in serieslist:
            seriesuidlist.append(ser.seriesuid)
        return Response(json.dumps(seriesuidlist))


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
