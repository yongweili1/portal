# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import os

from md.image3d.python.image3d_io import read_image, write_image
from md_segmentation3d.impl.vseg_cimpl import autoseg_load_model, autoseg_volume
# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView

from netbase.c_log import log
from service import series_svc, roi_svc, contour_svc
from utils.response_dto import ResponseDto
from utils.segmentation_helper import SegmentationHelper
from utils.uid_generator import UidGenerator

from config.path_cfg import file_path_ferry
import random


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
        if series_uid is None:
            return Response('请输入序列UID')

        mask_fp = file_path_ferry.volumePath + r'{}_mask.nii.gz'.format(series_uid)
        if not os.path.isfile(mask_fp):
            return ResponseDto(success=False, message='Please check mask file path.')

        # create a new roi for current segmentation
        roi_query = roi_svc.retrieve(series_uid)
        roi_uid = UidGenerator.roi_uid()
        roi = {
            'seriesuid': series_uid,
            'roiname': 'organ' + str(len(roi_query) + 1),
            'roicolor': self.random_color(),
            'roiuid': roi_uid
        }
        roi_svc.create(roi)
        # get current segmentation's contours
        contours = SegmentationHelper.mask_to_contours(mask_fp)
        success, msg = (True, '')
        for index in contours:
            success, msg = contour_svc.create(index, roi_uid, contours[index])
        return ResponseDto(data=roi_svc.single(roi_uid), success=success, message=msg)

    def random_color(self):
        colors = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f']
        color = "#ff"
        for i in range(4):
            color += colors[random.randint(0, 15)]
        return color

