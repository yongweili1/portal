# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from rest_framework.response import Response

from rest_framework.views import APIView
from serve.DBAccess.models import Roi, Series
from serve.DBAccess.serializer import RoiSerializer
from serve.util.generate_uid import GenerateUid


class RoiAPIView(APIView):

    def get(self, request):
        seriesuid = request.GET.get('seriesuid', None)
        if not seriesuid:
            return Response('参数不全')
        roi_query = Roi.objects.filter(seriesuid=seriesuid)
        roi_list = []
        for roi in roi_query:
            roi_dict = {}
            roi_dict['ROIId'] = roi.roiuid
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
        seriesuid = request.data.get('seriesuid', None)
        roiname = request.data.get('ROIName', None)
        roicolor = request.data.get('ROIColor', None)

        if seriesuid is None or roiname is None or roicolor is None:
            return Response('请携带完整的有效参数')

        if Roi.objects.filter(seriesuid=seriesuid, roiname=roiname):
            return Response('ROI命名重复')

        generateUid = GenerateUid()
        roiuid = generateUid.RoiUid()
        params = {
            'seriesuid': seriesuid,
            'roiname': roiname,
            'roicolor': roicolor,
            'roiuid': roiuid
        }

        try:
            roi = RoiSerializer(data=params)
            roi.is_valid(raise_exception=True)
            roi.save()
        except Exception as ex:
            print ex.message
            return Response('ROI save failed')

        roi_query = Roi.objects.filter(seriesuid=seriesuid)
        roi_list = []
        for roi in roi_query:
            roi_dict = {}
            roi_dict['ROIId'] = roi.roiuid
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
        roiuid = request.data.get('ROIId', None)
        roiname = request.data.get('ROIName', None)
        roicolor = request.data.get('ROIColor', None)

        if roiuid is None or roiname is None or roicolor is None:
            return Response('请携带完整的有效参数')

        params = {
            'roiname': roiname,
            'roicolor': roicolor
        }

        rois = Roi.objects.filter(roiuid=roiuid)
        if len(rois) == 0:
            return Response('该pid无对应的ROI')
        seriesuid = rois[0].seriesuid

        if Roi.objects.filter(seriesuid=seriesuid, roiname=roiname):
            return Response('ROI命名重复')

        try:
            Roi.objects.filter(roiuid=roiuid).update(**params)
        except Exception as e:
            return Response('ROI 更新失败')

        roi_query = Roi.objects.filter(seriesuid=seriesuid)
        roi_list = []
        for roi in roi_query:
            roi_dict = {}
            roi_dict['ROIId'] = roi.roiuid
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
            rois = Roi.objects.filter(roiuid=pid)
            if len(rois) == 0:
                return Response('该pid无对应的ROI')
            seruid = rois[0].seriesuid

            try:
                Roi.objects.filter(roiuid=pid).delete()
            except Exception as e:
                return Response('ROI 删除失败')

        roi_query = Roi.objects.filter(seriesuid=seruid)
        for roi in roi_query:
            roi_dict = {}
            roi_dict['ROIId'] = roi.roiuid
            roi_dict['ROIName'] = roi.roiname
            roi_dict['ROIColor'] = roi.roicolor
            roi_list.append(roi_dict)

        rsp = {
            'code': '200',
            'msg': 'success',
            'data': roi_list,
        }

        return Response(rsp)
