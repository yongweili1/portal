# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import json

from rest_framework.response import Response
from rest_framework.views import APIView

from serve.DBAccess.models import Series, Contour
from serve.util.generate_uid import GenerateUid
from serve.DBAccess.ss_serializer import ContourSerializer


class GraphElement(APIView):
    def __save_cps_to_file(self, cps, path):
        with open(path, 'wb') as f:
            f.write(json.dumps(cps))

    def __read_from_file(self, path):
        pass

    def get(self, request):
        roi_uid = request.GET.get('roi_uid', None)
        slice_index = request.GET.get('slice_index', None)
        cps = None
        try:
            cps = Contour.objects.filter(imageuid=slice_index, roiuid=roi_uid)
        except Exception as ex:
            print ex.message
        for cp in cps:
            series_query = Series.objects.filter(contouruid=cp.contouruid)
            if len(series_query) == 0:
                return Response('数据库无此seriesuid')
            volumepath = series_query[0].cpspath

        rsp = {
            'code': '200',
            'msg': 'success',
            'data': cps,
        }
        return Response(rsp)

    def post(self, request):
        roi_uid = request.data.get('roi_uid', None)
        slice_index = request.data.get('slice_index', None)
        contours = request.data.get('contours', None)

        # delete all contours for given roi and slice index
        try:
            Contour.objects.filter(imageuid=slice_index, roiuid=roi_uid).delete()
        except Exception as ex:
            print ex.message
            print 'contour delete error'

        for cps in contours:
            generateUid = GenerateUid()
            contour_uid = generateUid.ContourUid()
            file_name = contour_uid
            cpspath = r'D:\volume' + '\\' + file_name + '.txt'
            self.__save_cps_to_file(cps, cpspath)
            params = {
                'roiuid': roi_uid,
                'contouruid': contour_uid,
                'cpspath': cpspath,
                'imageuid': slice_index
            }
            try:
                contour = ContourSerializer(data=params)
                contour.is_valid(raise_exception=True)
                contour.save()
            except Exception as ex:
                print ex.message
                return Response('contour save failed')
        return Response('Update exist contour succeed.')

    def delete(self, request):
        roi_uid = request.GET.get('roi_uid', None)
        slice_index = request.GET.get('slice_index', None)
        # delete all contours for given roi and slice index
        try:
            Contour.objects.filter(imageuid=slice_index, roiuid=roi_uid).delete()
        except Exception as ex:
            print ex.message
            return Response('contour delete error')
        return Response('delete exist contour succeed.')
