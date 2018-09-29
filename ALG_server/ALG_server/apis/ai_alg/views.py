# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import sys
import md.image3d.python.image3d_io as cio
from rest_framework import status
from rest_framework.response import Response
from rest_framework.views import APIView
import json
from ai_server import AIServer
import pandas

from django.shortcuts import render

# Create your views here.

server = AIServer()


class Load(APIView):
    """
    load_volume
    :param request:
    :return:
    """
    def post(self, request):
        try:
            volume1 = request.data.get('volume')
            volume1 = volume1.encode('utf-8')
            # with open('./volume.nii.gz', 'wb') as f:
            #     f.write(volume1)
            # im = cio.read_image("/home/lyw/rxl5/pyproject/research_portal/portal/ALG_server/volume.nii.gz")
            volume1 = volume1.encode()
            volume3 = json.loads(volume1)
            del volume1

            seriesuid = request.data.get('seriesuid')
        except:
            return Response('Invalid parameters.', status=500)

        rst, msg = server.load_volume(volume3, seriesuid)
        del volume3
        status = 200 if rst else 500
        # volume = request.data.get('volume')
        # return Response("Load succeed", status)


class Calculate(APIView):

    def post(self, request):
        calculate = request.data.get('calculate')
        result = server.rib_fracture()
        status = 200 if result else 500
        return Response(result, status)
