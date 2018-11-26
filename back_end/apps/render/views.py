# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import os
import time
import json

from django.shortcuts import render
# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView

from service import contour_svc, roi_svc, series_svc, macro_svc

from utils.img_svr_connector import load_volume, get_image
from utils.response_dto import ResponseDto
from utils.uid_generator import UidGenerator
from utils.macro_recorder import MacroRecorder
from back_end.settings import STATIC_ROOT
from utils.volume_builder import VolumeBuilder
from config.path_cfg import file_path_ferry


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
            return ResponseDto(success=False, message='patientid is None')
        data, msg = series_svc.get_series_uids(patientid)
        if data:
            return ResponseDto(data)
        else:
            return ResponseDto(success=False, message=msg)


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
            MacroRecorder.macro_status = True
        elif macro_status == 'finish':
            macro_name = str(user_name) + str(time.time())
            scriptname = '{}.py'.format(macro_name)
            scriptpath = STATIC_ROOT + '/serve/macro/' + scriptname

            try:
                with open(scriptpath, 'a+') as f:
                    f.write(MacroRecorder.code_header + MacroRecorder.code)
            except:
                MacroRecorder.code = MacroRecorder.code
                return Response('脚本写入失败')

            MacroRecorder.macro_status = False
            MacroRecorder.code = ''

            data = {
                'scriptname': scriptname,
                'userid': 'anonymous',
                'scriptpath': scriptpath
            }

            success, msg = macro_svc.create(data)
            if not success:
                return ResponseDto(success=success, message=msg)

            return ResponseDto(macro_name)
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

        series, msg = series_svc.get_series_by_uid(seriesuid)
        if not series:
            return ResponseDto(success=False, message=msg)

        volumepath = series.seriespixeldatafilepath
        if not volumepath:
            return ResponseDto(success=False, message='volumepath error')

        params = {
            'seriesuid': seriesuid,
            'user_ip': user_ip,
            'volumepath': volumepath,
            'command': 'load',
            'server_name': 'image',
        }
        try:
            rst = load_volume(**params)
            return ResponseDto(data=rst.kwargs, success=rst.success, message=rst.comment)
        except Exception as e:
            return ResponseDto(success=False, message=e.message)

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
            builder = VolumeBuilder()
            volfilepath, seriesuid = builder.build(seriespath)
        except Exception as e:
            return Response('dicom文件不符合规范,创建volume失败')

        success, msg = series_svc.upload_volume(volfilepath, seriesuid)
        if not success:
            return ResponseDto(success=success, message=msg)

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

        return Response(rst.kwargs)


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

        img_server_rsp = rst.kwargs
        img_server_rsp_json = json.loads(img_server_rsp)

        slice_index = img_server_rsp_json['0']['slice_index']
        cps = contour_svc.retrieve(slice_index)
        slice_contours = []
        for cp in cps:
            file_path = cp.cpspath
            with open(file_path, 'rb') as f:
                contour = f.read()
                contour_json = json.loads(contour)
                slice_contours.append(contour_json)
                f.close()
            break
        kk = [slice_contours]
        img_server_rsp_json.get('0')[u'graphic']['contours'] = kk
        img_rsp = json.dumps(img_server_rsp_json)
        return Response(img_rsp)


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

        series, msg = series_svc.get_series_by_uid(seriesuid)
        if not series:
            return ResponseDto(success=False, message=msg)
        volumepath = series.seriespixeldatafilepath
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


class Contour(APIView):
    def __init__(self):
        pass

    def get(self, request):
        roi_uid = request.GET.get('roi_uid', None)
        slice_index = request.GET.get('slice_index', None)
        cps, msg = contour_svc.retrieve(slice_index, roi_uid)
        if cps:
            return ResponseDto(cps)
        else:
            return ResponseDto(success=False, message=msg)

    def post(self, request):
        roi_uid = request.data.get('roi_uid', None)
        slice_index = request.data.get('slice_index', None)
        contours = request.data.get('contours', None)
        success, msg = contour_svc.create(slice_index, roi_uid, contours)
        return ResponseDto(success=success, message=msg)

    def delete(self, request):
        roi_uid = request.GET.get('roi_uid', None)
        slice_index = request.GET.get('slice_index', None)
        success, msg = contour_svc.delete(slice_index, roi_uid)
        return ResponseDto(success=success, message=msg)


class Roi(APIView):
    def __init__(self):
        pass

    def get(self, request):
        series_uid = request.GET.get('seriesuid', None)
        if not series_uid:
            return ResponseDto(success=False, message='Missing required arguments.')
        rois = roi_svc.retrieve(series_uid)
        return ResponseDto(rois)

    def post(self, request):
        series_uid = request.data.get('seriesuid', None)
        name = request.data.get('name', None)
        color = request.data.get('color', None)

        if series_uid is None or name is None or name is None:
            return ResponseDto(success=False, message='Missing required arguments.')

        uid = UidGenerator.roi_uid()
        params = {
            'seriesuid': series_uid,
            'roiname': name,
            'roicolor': color,
            'roiuid': uid
        }

        success, msg = roi_svc.create(params)
        if not success:
            return ResponseDto(success=False, message=msg)
        else:
            return ResponseDto(uid)

    def put(self, request):
        uid = request.data.get('id', None)
        name = request.data.get('name', None)
        color = request.data.get('color', None)

        if uid is None or name is None or color is None:
            return ResponseDto(success=False, message='Missing required arguments.')

        params = {
            'roiuid': uid,
            'roiname': name,
            'roicolor': color
        }
        success, msg = roi_svc.update(params)
        return ResponseDto(success=success, message=msg)

    def delete(self, request):
        uids = request.GET.get('ids', None)
        if not uids:
            return ResponseDto(success=False, message='Arguments error.')
        uids = uids.split(',')
        success, msg = roi_svc.delete(uids)
        return ResponseDto(success=success, message=msg)
