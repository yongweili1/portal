# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import os
import time
import json

from django.shortcuts import render
# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView
from serve.DBAccess.upload_script_to_db import script
from serve.DBAccess.models import Series
from serve.util.connectImageServe import load_volume, get_image
from serve.util.macroRecording import Macro
from back_end.settings import STATIC_ROOT
from serve.DBAccess.models import Study
from serve.util.buildVolume import DicomToVolume
from serve.DBAccess.upload_vol_to_db import UploadVolume
from serve.static_parameters.setFilePath import file_path_ferry
from serve.DBAccess.models import Series, Contour


class Home(APIView):

    def get(self, request):
        # user_ip = request.META.get('REMOTE_ADDR', None)

        print('路由分发后:{}'.format(str(time.time())))
        print('响应经由中间件前:{}'.format(str(time.time())))
        return render(request, 'main.html')


class GetSeriesUidList(APIView):

    def get(self, request):
        """
        provide a list of seriesuid
        :param request:
        :return:a list of seriesuid
        """
        patientid = request.GET.get('patientid', None)
        if patientid is None:
            return Response('请输入patientid')
        studylist = Study.objects.filter(patientid=patientid)
        serieslist = []
        for study in studylist:
            series_list = Series.objects.filter(studyuid=study.studyuid)
            serieslist += series_list
        # serieslist = Series.objects.all()
        seriesuidlist = []
        for ser in serieslist:
            seriesuidlist.append(ser.seriesuid)
        return Response(seriesuidlist)


class MacroRecording(APIView):

    def get(self, request):
        """
        Opening and closing macro recording, save script
        :param macro_status: the status of macro recording
        :return:
        """
        macro_status = request.GET.get('macro_status', None)
        user_name = request.user
        if macro_status == 'start':
            Macro.macro_status = True
        elif macro_status == 'finish':
            macro_name = str(user_name) + str(time.time())
            scriptname = '{}.py'.format(macro_name)
            scriptpath = STATIC_ROOT + '/serve/macro/' + scriptname

            try:
                with open(scriptpath, 'a+') as f:
                    f.write(Macro.code_header + Macro.code)
            except:
                Macro.code = Macro.code
                return Response('脚本写入失败')

            Macro.macro_status = False
            Macro.code = ''

            data = {
                'scriptname': scriptname,
                'userid': 'anonymous',
                'scriptpath': scriptpath
            }

            result = script.uploadscript(**data)

            if result is 'fail':
                return Response('upload script fail ！')

            return Response(macro_name)

        else:
            pass

        return Response('OK')


class WindowSize(APIView):

    def post(self, request):
        sizedata = request.data.get('view_size', None)
        user_ip = request.META.get('REMOTE_ADDR', None)

        if sizedata is None:
            return Response('resize参数不完整')
        print(sizedata)

        params = {
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'resize',
            'view_size': sizedata
        }

        try:
            rst = get_image(**params)
        except Exception as e:
            return Response(e.message)

        if rst.success is False:
            return Response(rst.comment)

        return Response('success')


class LoadVolume(APIView):

    def get(self, request):
        """
        the client request loads a series
        :param seriesuid: series uid
        :return:boolean true for success, false for failure
        """
        seriesuid = request.GET.get('seriesuid', None)
        user_ip = request.META.get('REMOTE_ADDR', None)

        series_query = Series.objects.filter(seriesuid=seriesuid)
        if len(series_query) == 0:
            return Response('数据库无此seriesuid')
        volumepath = series_query[0].seriespixeldatafilepath

        if not volumepath:
            return Response('rebuild')

        params = {
            'seriesuid': seriesuid,
            'user_ip': user_ip,
            'volumepath': volumepath,
            'command': 'load',
            'server_name': 'image',
        }
        try:
            rst = load_volume(**params)
        except IOError:
            return Response('rebuild')

        if rst.success is False:
            return Response(rst.comment)

        return Response(rst.kwargs)

    def put(self, request):

        request_data = request.data
        user_ip = request.META.get('REMOTE_ADDR', None)

        if request_data is None:
            return Response('请携带有效的seriesuid')
        seriesuid = request_data['params']['updates'][0]['value']
        seriespath = file_path_ferry.splitDicomPath + str(seriesuid)

        if not os.path.exists(seriespath):
            return Response('请确认series数据是否存在')

        if len(os.listdir(seriespath)) <= 1:
            return Response('series下的dicom文件单一，无法build volume')

        try:
            buildvol = DicomToVolume()
            volfilepath, seriesuid = buildvol.dicom_to_volume(seriespath)
        except Exception as e:
            return Response('dicom文件不符合规范,创建volume失败')

        try:
            UploadVolume(volfilepath, seriesuid)
        except Exception as e:
            return Response('Volume入库失败')

        params = {
            'seriesuid': seriesuid,
            'user_ip': user_ip,
            'volumepath': volfilepath,
            'command': 'load',
            'server_name': 'image',
        }
        try:
            rst = load_volume(**params)
        except IOError:
            return Response('fail')

        if rst.success is False:
            return Response(rst.comment)

        return Response('success')

    def delete(self, request):
        """
        the client request unload volume data from memory
        :param seriesuid: series uid
        :return:boolean true for success, false for failure
        """
        seriesuid = request.GET.get('seriesuid', None)
        user_ip = request.META.get('REMOTE_ADDR', None)
        if seriesuid is None:
            return Response('请输入序列UID')

        params = {
            'seriesuid': seriesuid,
            'user_ip': user_ip,
            'command': 'unload',
            'server_name': 'image',
        }
        try:
            rst = get_image(**params)
        except Exception as e:
            return Response(e.message)

        if rst.success is False:
            return Response(rst.comment)

        return Response('success')


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
        seriesuid = request.GET.get('seriesuid', None)
        width = request.GET.get('width', None)
        height = request.GET.get('height', None)
        display_view = request.GET.get('display_view', 'all')
        user_ip = request.META.get('REMOTE_ADDR', None)
        # request_server = request.path
        # request_server = request_server.split("/")[-2]

        if width is None or height is None:
            return Response('请输入完整的请求数据')

        params = {
            'seriesuid': seriesuid,
            'width': width,
            'height': height,
            'display_view': display_view,
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'show',
        }

        try:
            rst = get_image(**params)
        except Exception as e:
            return Response(e.message)

        if rst.success is False:
            return Response(rst.comment)

        a = rst.kwargs
        b = json.loads(a)
        b.get('0')[u'graphic']['contours'] = [[[{"y": 252, "x": 690}, {"y": 252, "x": 687}, {"y": 252, "x": 681},
                                                {"y": 252, "x": 671}, {"y": 244, "x": 649}, {"y": 233, "x": 627},
                                                {"y": 222, "x": 607}, {"y": 211, "x": 594}, {"y": 200, "x": 582},
                                                {"y": 188, "x": 574}, {"y": 175, "x": 568}, {"y": 164, "x": 566},
                                                {"y": 153, "x": 566}, {"y": 146, "x": 566}, {"y": 138, "x": 566},
                                                {"y": 132, "x": 566}, {"y": 124, "x": 570}, {"y": 116, "x": 573},
                                                {"y": 109, "x": 578}, {"y": 105, "x": 580}, {"y": 100, "x": 584},
                                                {"y": 97, "x": 590}, {"y": 95, "x": 594}, {"y": 91, "x": 602},
                                                {"y": 90, "x": 610}, {"y": 86, "x": 630}, {"y": 85, "x": 643},
                                                {"y": 83, "x": 661}, {"y": 83, "x": 682}, {"y": 83, "x": 704},
                                                {"y": 85, "x": 721}, {"y": 88, "x": 733}, {"y": 91, "x": 743},
                                                {"y": 94, "x": 750}, {"y": 99, "x": 760}, {"y": 103, "x": 765},
                                                {"y": 107, "x": 771}, {"y": 110, "x": 776}, {"y": 114, "x": 779},
                                                {"y": 121, "x": 786}, {"y": 127, "x": 791}, {"y": 133, "x": 795},
                                                {"y": 140, "x": 797}, {"y": 146, "x": 800}, {"y": 155, "x": 804},
                                                {"y": 163, "x": 806}, {"y": 169, "x": 806}, {"y": 174, "x": 807},
                                                {"y": 179, "x": 807}, {"y": 181, "x": 807}, {"y": 186, "x": 807},
                                                {"y": 190, "x": 807}, {"y": 193, "x": 807}, {"y": 196, "x": 807},
                                                {"y": 202, "x": 806}, {"y": 210, "x": 802}, {"y": 215, "x": 799},
                                                {"y": 219, "x": 798}, {"y": 222, "x": 794}, {"y": 224, "x": 792},
                                                {"y": 226, "x": 791}, {"y": 228, "x": 790}, {"y": 230, "x": 788},
                                                {"y": 232, "x": 786}, {"y": 233, "x": 784}, {"y": 235, "x": 782},
                                                {"y": 236, "x": 779}, {"y": 238, "x": 777}, {"y": 238, "x": 776},
                                                {"y": 240, "x": 772}, {"y": 241, "x": 771}, {"y": 242, "x": 767},
                                                {"y": 244, "x": 762}, {"y": 244, "x": 759}, {"y": 245, "x": 754},
                                                {"y": 246, "x": 749}, {"y": 248, "x": 745}, {"y": 248, "x": 741},
                                                {"y": 248, "x": 737}, {"y": 249, "x": 732}, {"y": 250, "x": 726},
                                                {"y": 250, "x": 723}, {"y": 251, "x": 719}, {"y": 252, "x": 716},
                                                {"y": 253, "x": 711}, {"y": 253, "x": 709}, {"y": 253, "x": 707},
                                                {"y": 253, "x": 705}, {"y": 253, "x": 702}, {"y": 253, "x": 698},
                                                {"y": 253, "x": 695}, {"y": 253, "x": 692}, {"y": 252, "x": 690}]]]
        c = json.dumps(b)

        return Response(c)


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

        params = {
            'colormode': colormode,
            'width': width,
            'height': height,
            'display_view': display_view,
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'color',
        }

        try:
            rst = get_image(**params)
        except Exception as e:
            return Response(e.message)

        if rst.success is False:
            return Response(rst.comment)

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
        user_ip = request.META.get('REMOTE_ADDR', None)
        request_server = request.path
        request_server = request_server.split("/")[-2]
        if width is None or height is None:
            return Response('请输入完整的请求数据')

        params = {
            'delta': delta,
            'width': width,
            'height': height,
            'focus_view': focus_view,
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'page',
        }

        try:
            rst = get_image(**params)
        except Exception as e:
            return Response(e.message)

        if rst.success is False:
            return Response(rst.comment)





        a = rst.kwargs
        b = json.loads(a)

        slice_index = b['0']['slice_index']
        cps = None
        try:
            cps = Contour.objects.filter(imageuid=slice_index)
        except Exception as ex:
            print ex.message
        pps = []
        for cp in cps:
            file_path = cp.cpspath
            with open(file_path, 'rb') as f:
                pp = f.read()
                p = json.loads(pp)
                pps.append(p)
                f.close()
            break
        kk = [pps]
        b.get('0')[u'graphic']['contours'] = kk
        c = json.dumps(b)
        return Response(c)


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
        pos_pre = request.GET.get('pos_pre', None)
        pos_cur = request.GET.get('pos_cur', None)
        focus_view = request.GET.get('focus_view', None)
        user_ip = request.META.get('REMOTE_ADDR', None)
        if pos_pre  is None or pos_cur is None:
            return Response('请输入完整的请求数据')

        params = {
            'pos_pre': pos_pre,
            'pos_cur': pos_cur,
            'focus_view': focus_view,
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'pan',
        }

        try:
            rst = get_image(**params)
        except Exception as e:
            return Response(e.message)

        if rst.success is False:
            return Response(rst.comment)

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

        params = {
            'cursor': cursor,
            'width': width,
            'height': height,
            'focus_view': focus_view,
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'roll',
        }

        try:
            rst = get_image(**params)
        except Exception as e:
            return Response(e.message)

        if rst.success is False:
            return Response(rst.comment)

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
        pos_pre = request.GET.get('pos_pre', None)
        pos_cur = request.GET.get('pos_cur', None)
        focus_view = request.GET.get('focus_view', None)
        user_ip = request.META.get('REMOTE_ADDR', None)
        if pos_pre is None or pos_cur is None:
            return Response('请输入完整的请求数据')

        params = {
            'pos_pre': pos_pre,
            'pos_cur': pos_cur,
            'focus_view': focus_view,
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'rotate',
        }

        try:
            rst = get_image(**params)
        except Exception as e:
            return Response(e.message)

        if rst.success is False:
            return Response(rst.comment)

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
        zoom_factor = request.GET.get('zoom_factor', '0')
        focus_view = request.GET.get('focus_view', None)
        user_ip = request.META.get('REMOTE_ADDR', None)
        if focus_view is None:
            return Response('请输入完整的请求数据')

        params = {
            'zoom_factor': zoom_factor,
            'focus_view': focus_view,
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'zoom',
        }

        try:
            rst = get_image(**params)
        except Exception as e:
            return Response(e.message)

        if rst.success is False:
            return Response(rst.comment)

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
        user_ip = request.META.get('REMOTE_ADDR', None)

        params = {
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'reset',
        }

        try:
            rst = get_image(**params)
        except Exception as e:
            return Response(e.message)

        if rst.success is False:
            return Response(rst.comment)

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
        ww_factor = request.GET.get('ww_factor', None)
        wl_factor = request.GET.get('wl_factor', None)
        focus_view = request.GET.get('focus_view', None)
        user_ip = request.META.get('REMOTE_ADDR', None)
        if ww_factor is None or wl_factor is None:
            return Response('请输入完整的请求数据')

        params = {
            'ww_factor': ww_factor,
            'wl_factor': wl_factor,
            'focus_view': focus_view,
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'wcww',
        }

        try:
            rst = get_image(**params)
        except Exception as e:
            return Response(e.message)

        if rst.success is False:
            return Response(rst.comment)

        return Response(rst.kwargs)


class ChangeWindowTwo(APIView):

    def get(self, request):
        """
        Change window center and window width
        :param ww:
        :param wl:
        :return: rgb image data
        """
        ww = request.GET.get('ww', None)
        wl = request.GET.get('wl', None)
        user_ip = request.META.get('REMOTE_ADDR', None)
        if ww is None or wl is None:
            return Response('请输入完整的请求数据')

        params = {
            'ww': ww,
            'wl': wl,
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'wwwl',
        }

        try:
            rst = get_image(**params)
        except Exception as e:
            return Response('服务间数据传输失败')

        if rst.success is False:
            return Response(rst.comment)

        return Response(rst.kwargs)


class CrossLineLocation(APIView):

    def get(self, request):
        cross_point = request.GET.get('cross_point', None)
        focus_view = request.GET.get('focus_view', None)
        display_view = request.GET.get('display_view', None)
        user_ip = request.META.get('REMOTE_ADDR', None)

        if not cross_point or not focus_view or not user_ip or not display_view:
            return Response('请求参数不完整')

        params = {
            'cross_point': cross_point,
            'focus_view': focus_view,
            'display_view': display_view,
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'locate',
        }

        try:
            rst = get_image(**params)
        except Exception as e:
            return Response(e.message)

        if rst.success is False:
            return Response(rst.comment)

        return Response(rst.kwargs)


class RunSript(APIView):

    def get(self, request):
        seriesuid = request.GET.get('seriesuid', '1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373')
        scriptname = request.GET.get('scriptname', 'AnonymousUser1540275438.76')
        if not seriesuid or not scriptname:
            return Response('请输入有效的参数')

        series_query = Series.objects.filter(seriesuid=seriesuid)
        if len(series_query) == 0:
            return Response('数据库无此seriesuid')
        volumepath = series_query[0].seriespixeldatafilepath

        try:
            command = 'python static/macro/{}.py {} {}'.format(scriptname, seriesuid, volumepath)
            r = os.popen(command)
            info = r.readlines()
            print(info)
        except:
            return Response('script执行异常')

        return Response('OK')


class SetCenter(APIView):

    def get(self, request):
        user_ip = request.META.get('REMOTE_ADDR', None)
        params = {
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'center',
        }

        try:
            rst = get_image(**params)
        except Exception as e:
            return Response(e.message)

        if rst.success is False:
            return Response(rst.comment)

        return Response(rst.kwargs)
