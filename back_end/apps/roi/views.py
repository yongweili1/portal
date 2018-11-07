# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from rest_framework.response import Response

from rest_framework.views import APIView
from serve.DBAccess.models import Roi, Series


class RoiAPIView(APIView):

    def get(self, request):
        seriesuid = request.GET.get('seriesUid', None)
        if not seriesuid:
            return Response('参数不全')
        roi_query = Roi.objects.filter(seriesuid=seriesuid)
        if not roi_query:
            return Response('该seriesuid下无对应的ROI')
        roi_list = []
        for roi in roi_query:
            roi_dict = {}
            roi_dict['ROIId'] = roi.pid
            roi_dict['ROIName'] = roi.roiname
            roi_dict['ROIColor'] = roi.roicolor
            roi_list.append(roi_dict)

        rsp = {
            'code': '200',
            'msg': 'success',
            'data': roi_list
        }

        return Response(rsp)

    def post(self, request):
        seriesuid = request.data.get('seriesUid', None)
        roiname = request.data.get('ROIName', None)
        roicolor = request.data.get('ROIColor', None)

        if seriesuid is None or roiname is None or roicolor is None:
            return Response('请携带完整的有效参数')
        try:
            seriesobj = Series.objects.get(seriesuid=seriesuid)
        except Exception as e:
            return Response('外键seriesuid无对应的数据对象')

        params = {
            'seriesuid': seriesobj,
            'roiname': roiname,
            'roicolor': roicolor
        }

        try:
            roiobj = Roi.objects.create(**params)
            roiobj.save()
        except Exception as e:
            return Response('ROI 提交失败')

        roi_query = Roi.objects.filter(seriesuid=seriesuid)
        if not roi_query:
            return Response('该seriesuid下无对应的ROI')
        roi_list = []
        for roi in roi_query:
            roi_dict = {}
            roi_dict['ROIId'] = roi.pid
            roi_dict['ROIName'] = roi.roiname
            roi_dict['ROIColor'] = roi.roicolor
            roi_list.append(roi_dict)

        rsp = {
            'code': '200',
            'msg': 'success',
            'data': roi_list,
        }

        return Response(rsp)

    def put(self, request):
        pid = request.data.get('ROIId', None)
        roiname = request.data.get('ROIName', None)
        roicolor = request.data.get('ROIColor', None)

        if pid is None or roiname is None or roicolor is None:
            return Response('请携带完整的有效参数')

        params = {
            'roiname': roiname,
            'roicolor': roicolor
        }

        try:
            Roi.objects.filter(pid=pid).update(**params)
        except Exception as e:
            return Response('ROI 更新失败')

        rois = Roi.objects.filter(pid=pid)
        if len(rois) == 0:
            return Response('该pid无对应的ROI')
        seriesuid = rois[0].seriesuid

        roi_query = Roi.objects.filter(seriesuid=seriesuid)
        if not roi_query:
            return Response('该seriesuid下无对应的ROI')
        roi_list = []
        for roi in roi_query:
            roi_dict = {}
            roi_dict['ROIId'] = roi.pid
            roi_dict['ROIName'] = roi.roiname
            roi_dict['ROIColor'] = roi.roicolor
            roi_list.append(roi_dict)

        rsp = {
            'code': '200',
            'msg': 'success',
            'data': roi_list,
        }

        return Response(rsp)

    def delete(self, request):
        pids = request.GET.get('ROIId', None)
        if not pids:
            return Response('请携带有效参数')

        pids = pids.split(',')
        roi_list = []
        seruid = ''

        for pid in pids:
            rois = Roi.objects.filter(pid=pid)
            if len(rois) == 0:
                return Response('该pid无对应的ROI')
            seruid = rois[0].seriesuid

            try:
                Roi.objects.filter(pid=pid).delete()
            except Exception as e:
                return Response('ROI 删除失败')

        roi_query = Roi.objects.filter(seriesuid=seruid)
        if not roi_query:
            rsp = {
                'code': '20001',
                'msg': 'success',
                'data': '该seriesuid下仅有一个ROI已被删除',
            }
            return Response(rsp)
        for roi in roi_query:
            roi_dict = {}
            roi_dict['ROIId'] = roi.pid
            roi_dict['ROIName'] = roi.roiname
            roi_dict['ROIColor'] = roi.roicolor
            roi_list.append(roi_dict)

        rsp = {
            'code': '200',
            'msg': 'success',
            'data': roi_list,
        }

        return Response(rsp)
