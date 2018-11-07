# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from rest_framework.response import Response

from rest_framework.views import APIView
from serve.DBAccess.models import Roi


class RoiAPIView(APIView):

    def get(self, request):
        seriesuid = request.GET.get('seriesUid', None)
        if not seriesuid:
            return Response('参数不全')
        roi_query = Roi.objects.filter(seriesuid=seriesuid)
        roi_dict = {}
        roi_list = []
        for roi in roi_query:
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

        params = {
            'seriesuid': seriesuid,
            'roiname': roiname,
            'roicolor': roicolor
        }

        Roi.objects.create(**params)

        rsp = {
            'code': '200',
            'msg': 'success',
            'data': ''
        }

        return Response(rsp)

    def put(self, request):
        pid = request.data.get('ROIId', None)
        roiname = request.data.get('ROIName', None)
        roicolor = request.data.get('ROIColor', None)
        params = {
            'roiname': roiname,
            'roicolor': roicolor
        }

        Roi.objects.filter(pid=pid).update(**params)

        rsp = {
            'code': '200',
            'msg': 'success',
            'data': ''
        }

        return Response(rsp)
