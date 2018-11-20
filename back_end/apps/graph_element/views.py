# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from rest_framework.response import Response
from rest_framework.views import APIView

from serve.DBAccess.contour_crud import ContourCrud


class GraphElement(APIView):
    def __init__(self):
        self.contour_crud = ContourCrud()

    def get(self, request):
        roi_uid = request.GET.get('roi_uid', None)
        slice_index = request.GET.get('slice_index', None)
        cps = self.contour_crud.Retrieve(slice_index, roi_uid)
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
        if len(contours) > 0:
            self.contour_crud.Delete(slice_index, roi_uid)
            for cps in contours:
                self.contour_crud.Insert(slice_index, roi_uid, cps)
        return Response('Update exist contour succeed.')

    def delete(self, request):
        roi_uid = request.GET.get('roi_uid', None)
        slice_index = request.GET.get('slice_index', None)

        self.contour_crud.Delete(slice_index, roi_uid)
        return Response('delete exist contour succeed.')
