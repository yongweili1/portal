# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.shortcuts import render
from rest_framework.response import Response

from rest_framework.views import APIView
from serve.util.models import Roi


class RoiAPIView(APIView):

    def get(self, request):
        seriesuid = request.GET.get('seriesUid', None)
        roiname = request.GET.get('ROIName', None)
        roicolor = request.GET.get('ROIColor', None)

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
