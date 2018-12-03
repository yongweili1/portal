# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import json
import os
import time

from config.path_cfg import file_path_ferry
from django.shortcuts import render
from rest_framework.views import APIView
from service import contour_svc, roi_svc, series_svc
from utils.img_svr_connector import sync_send_command
from utils.response_dto import ResponseDto
from utils.uid_generator import UidGenerator
from utils.volume_builder import VolumeBuilder


class Home(APIView):
    def get(self, request):
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


class WindowSize(APIView):
    def post(self, request):
        try:
            sizedata = request.data.get('view_size', None)
            if sizedata is None:
                return ResponseDto(success=False, message='sizedata is None')
            return sync_send_command('resize', view_size=sizedata)
        except Exception as e:
            return ResponseDto(success=False, message=e.message)


class LoadVolume(APIView):
    def get(self, request):
        """
        the client request loads a series
        :param seriesuid: series uid
        :return:boolean true for success, false for failure
        """
        try:
            seriesuid = request.GET.get('seriesuid', None)
            series, msg = series_svc.get_series_by_uid(seriesuid)
            if series is None:
                return ResponseDto(success=False, message=msg)

            return sync_send_command('load', volumepath=series.seriespixeldatafilepath)
        except Exception as e:
            return ResponseDto(success=False, message=e.message)

    def put(self, request):
        request_data = request.data

        if request_data is None:
            return ResponseDto(success=False, message='请携带有效的seriesuid')
        seriesuid = request_data['params']['updates'][0]['value']
        seriespath = file_path_ferry.splitDicomPath + str(seriesuid)

        if not os.path.exists(seriespath):
            return ResponseDto(success=False, message='请确认series数据是否存在')

        if len(os.listdir(seriespath)) <= 1:
            return ResponseDto(success=False, message='series下的dicom文件单一，无法build volume')

        try:
            builder = VolumeBuilder()
            volfilepath, seriesuid = builder.build(seriespath)
        except Exception as e:
            return ResponseDto(success=True, message='dicom文件不符合规范,创建volume失败')

        success, msg = series_svc.upload_volume(volfilepath, seriesuid)
        if not success:
            return ResponseDto(success=success, message=msg)

        try:
            return sync_send_command('load', volumepath=volfilepath)
        except Exception as e:
            return ResponseDto(success=False, message=e.message)

    def delete(self, request):
        """
        the client request unload volume data from memory
        :param seriesuid: series uid
        :return:boolean true for success, false for failure
        """
        try:
            seriesuid = request.GET.get('seriesuid', None)
            if seriesuid is None:
                return ResponseDto(success=False, message='seriesuid is None')

            return sync_send_command('unload', seriesuid=seriesuid)
        except Exception as e:
            return ResponseDto(success=False, message=e.message)


class GetImage(APIView):
    def get(self, request):
        """
        the client request loads a series and get images
        :return: rgb image data
        """
        try:
            return sync_send_command('show')
        except Exception as e:
            return ResponseDto(success=False, message=e.message)


class ChangeColor(APIView):
    def get(self, request):
        try:
            colormode = request.GET.get('colormode', '0')
            display_view = request.GET.get('display_view', 'all')
            return sync_send_command('color', colormode=colormode, display_view=display_view)
        except Exception as e:
            return ResponseDto(success=True, message=e.message)


class TurnPage(APIView):
    def get(self, request):
        """
        Page turning
        :param delta: cursor delta
        :param focus_view: current focused view, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal
        :return: rgb image data
        """
        try:
            delta = request.GET.get('delta', '0')
            focus_view = request.GET.get('focus_view', None)
            return sync_send_command('page', delta=delta, focus_view=focus_view)
        except Exception as e:
            return ResponseDto(success=False, message=e.message)


class Pan(APIView):
    def get(self, request):
        """
        translation
        :param shift: center shift, like [1,1,2], you should pass it like `'1,1,2'`
        :param focus_view: current focused view, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal
        :param display_view: need to displayed on screen, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal, 'all' for all view
        :return: rgb image data
        """
        try:
            pos_pre = request.GET.get('pos_pre', None)
            pos_cur = request.GET.get('pos_cur', None)
            focus_view = request.GET.get('focus_view', None)
            if pos_pre is None or pos_cur is None:
                return ResponseDto(success=False, message='请输入完整的请求数据')
            return sync_send_command('pan', pos_pre=pos_pre, pos_cur=pos_cur, focus_view=focus_view)
        except Exception as e:
            return ResponseDto(success=True, message=e.message)


class Roll(APIView):
    def get(self, request):
        """
        roll
        :param cursor: the 3d world coordinate of a cursor, like [1,1,2], you should pass it like `'1,1,2'`
        :param focus_view: current focused view, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal
        :return: rgb image data
        """
        try:
            cursor = request.GET.get('cursor', '0')
            focus_view = request.GET.get('focus_view', None)
            return sync_send_command('roll', cursor=cursor, focus_view=focus_view)
        except Exception as e:
            return ResponseDto(success=False, message=e.message)


class Rotate(APIView):
    def get(self, request):
        """
        Rotate
        :param angle: rotate angle
        :param focus_view: current focused view, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal
        :return: rgb image data
        """
        try:
            pos_pre = request.GET.get('pos_pre', None)
            pos_cur = request.GET.get('pos_cur', None)
            focus_view = request.GET.get('focus_view', None)
            if pos_pre is None or pos_cur is None or focus_view is None:
                return ResponseDto(success=False, message='请输入完整的请求数据')

            return sync_send_command('rotate', pos_pre=pos_pre,
                                  pos_cur=pos_cur, focus_view=focus_view)
        except Exception as e:
            return ResponseDto(success=False, message=e.message)


class Zoom(APIView):
    def get(self, request):
        """
        Zoom
        :param shift: zoom factor shift
        :param focus_view: current focused view, 'transverse' for transverse,
              'saggital' for saggital, 'coronal' for coronal
        :return: rgb image data
        """
        try:
            zoom_factor = request.GET.get('zoom_factor', '0')
            focus_view = request.GET.get('focus_view', None)
            if focus_view is None:
                return ResponseDto(success=False, message='请输入完整的请求数据')
            return sync_send_command('zoom', zoom_factor=zoom_factor, focus_view=focus_view)
        except Exception as e:
            return ResponseDto(success=False, message=e.message)


class ReSetVolume(APIView):
    def get(self, request):
        """
        Reset voulme data
        :return: True or False
        """
        try:
            return sync_send_command('reset')
        except Exception as e:
            return ResponseDto(success=False, message=e.message)


class ChangeWindow(APIView):
    def get(self, request):
        """
        Change window center and window width
        :param ww: ww
        :param wl: wl
        :return: rgb image data
        """
        try:
            ww = request.GET.get('ww', None)
            wl = request.GET.get('wl', None)
            if ww is None or wl is None:
                return ResponseDto(success=False, message='请输入完整的请求数据')
            return sync_send_command('wwwl', ww=ww, wl=wl)
        except Exception as e:
            return ResponseDto(success=False, message=e.message)


class CrossLineLocation(APIView):
    def get(self, request):
        try:
            cross_point = request.GET.get('cross_point', None)
            focus_view = request.GET.get('focus_view', None)

            if not cross_point or not focus_view:
                return ResponseDto(success=False, message='请求参数不完整')
            return sync_send_command('locate', cross_point=cross_point, focus_view=focus_view)
        except Exception as e:
            return ResponseDto(success=False, message=e.message)


class SetCenter(APIView):
    def get(self, request):
        try:
            return sync_send_command('center')
        except Exception as e:
            return ResponseDto(success=False, message=e.message)


class Contour(APIView):

    def get(self, request):
        roi_uids = request.GET.get('roi_uids', None)
        slice_index = request.GET.get('slice_index', None)
        roi_uids = roi_uids.split(',')
        contours, msg = contour_svc.retrieve(slice_index, roi_uids)
        return ResponseDto(data=contours)

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
