# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from rest_framework.views import APIView

from netbase.c_log import log
from service import series_svc, contour_svc
from utils.response_dto import ResponseDto

from config.path_cfg import file_path_ferry


class LoadAlg(APIView):
    def get(self, request):
        serid = request.GET.get('seriesuid', None)
        if serid is None:
            return ResponseDto(success=False, message='请输入序列UID')

        series, msg = series_svc.get_series_by_uid(serid)
        if series is None:
            return ResponseDto(success=False, message=msg)

        volumepath = series.seriespixeldatafilepath
        resp = None  # resp: Load succeed
        return ResponseDto(resp)


class GetAlgResult(APIView):
    def get(self, request):
        series_uid = request.GET.get('seriesuid', None)
        roi_uid = request.GET.get('roiuid', None)
        mask_fp = file_path_ferry.volumePath + r'{}_mask.nii.gz'.format(series_uid)
        contour_svc.contour_2_mask(mask_fp, roi_uid)
        return ResponseDto()

    def post(self, request):
        series_uid = request.data.get('seriesuid', None)
        if series_uid is None:
            return ResponseDto(success=False, message='请输入序列UID')

        # segmentation
        mask_fp, msg = contour_svc.seg(series_uid)
        if mask_fp is None:
            return ResponseDto(success=False, message=msg)

        # mask to contours
        roi, msg = contour_svc.mask_2_contour(series_uid, mask_fp)
        if roi is None:
            return ResponseDto(success=False, message=msg)
        else:
            return ResponseDto(roi)



