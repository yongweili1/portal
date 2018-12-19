# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import random

from rest_framework.views import APIView

from netbase.c_log import log
from service import series_svc, contour_svc, roi_svc
from utils.response_dto import ResponseDto, FileResponseDto

from config.path_cfg import file_path_ferry
from utils.uid_generator import UidGenerator


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
        success, msg = contour_svc.contour_2_mask(mask_fp, roi_uid)
        if not success:
            return ResponseDto(success=False, message=msg)

        return FileResponseDto(mask_fp)

    def post(self, request):
        series_uid = request.data.get('seriesuid', None)
        organs = request.data.get('organs', None)

        if series_uid is None or organs is None or len(organs) == 0:
            return ResponseDto(success=False, message='请输入序列UID')

        for organ in organs:
            # segmentation
            mask_fp, msg = contour_svc.seg(series_uid)
            if mask_fp is None:
                return ResponseDto(success=False, message=msg)

            # create a new roi for current segmentation
            roi_uid = UidGenerator.roi_uid()
            roi = {
                'seriesuid': series_uid,
                'roiname': organ['name'],
                'roicolor': self.random_color(),
                'roiuid': roi_uid
            }
            duplicate, uids = roi_svc.duplicate(series_uid, organ['name'])

            if duplicate:
                roi_svc.delete(uids)
            success, msg = roi_svc.create(roi)
            if not success:
                return ResponseDto(success=False, message=msg)

            # mask to contours
            success, msg = contour_svc.mask_2_contour(roi_uid, mask_fp)
            if not success:
                return ResponseDto(success=False, message=msg)

        return ResponseDto(roi_svc.retrieve(series_uid))

    def random_color(self):
        colors = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f']
        color = "#ff"
        for i in range(4):
            color += colors[random.randint(0, 15)]
        return color
