# -*- coding: utf-8 -*-
from __future__ import unicode_literals

# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView
from serve.DBAccess.models import Series


class LoadAlg(APIView):
    def get(self, request):
        serid = request.GET.get('seriesuid', None)
        if serid is None:
            return Response('请输入序列UID')

        seriesobject = Series.objects.filter(seriesuid=serid)
        if len(seriesobject) == 0:
            return Response('请输入正确的序列ID')

        volumepath = seriesobject[0].seriespixeldatafilepath

        resp = None  # resp: Load succeed

        return Response(resp)


class GetAlgResult(APIView):

    def get(self, request):

        resp = None
        return Response(resp)
