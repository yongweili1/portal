# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from md.image3d.python.image3d_io import read_image, write_image
from md_segmentation3d.impl.vseg_cimpl import autoseg_load_model, autoseg_volume
# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView

from service import series_svc, roi_svc
from utils.response_dto import ResponseDto


class LoadAlg(APIView):
    def get(self, request):
        serid = request.GET.get('seriesuid', None)
        if serid is None:
            return Response('请输入序列UID')

        series, msg = series_svc.get_series_by_uid(serid)
        if series is None:
            return ResponseDto(success=False, message=msg)

        volumepath = series.seriespixeldatafilepath
        resp = None  # resp: Load succeed
        return Response(resp)


class GetAlgResult(APIView):

    def get(self, request):

        resp = None
        return Response(resp)

    def post(self, request):
        series_uid = request.data.get('seriesuid', None)
        # roiname = request.data.get('ROIName', None)
        # roicolor = request.data.get('ROIColor', None)

        if series_uid is None:
            return Response('请输入序列UID')

        series, msg = series_svc.get_series_by_uid(series_uid)
        if series is None:
            return ResponseDto(success=False, message=msg)

        volume_path = series.seriespixeldatafilepath

        im = read_image(volume_path)
        try:
            model = autoseg_load_model(r'D:\segmentation_model\VSEG_Heart_20180611_01', 0)
            seg = autoseg_volume(im, model)
            write_image(seg, r'D:\segmentation_model\test_result.nii.gz')
        except Exception as ex:
            print ex.message

        roi_query = roi_svc.retrieve(series_uid)
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
