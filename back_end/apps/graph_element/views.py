# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import time

from django.shortcuts import render

# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView
from back_end.settings import STATIC_ROOT
from upload_dot_to_db import overlay


class GraphElement(APIView):

    def post(self, request):
        """
        upload overlay to local file and save path to DB
        :param seriesuid:the id of volume belong to overlay
        :param overlaytype: shape type of overlay
        :param dotpositon: point coordinate
        :return:
        """
        seriesuid = request.POST.get('seriesuid', None)
        overlaytype = request.POST.get('overlaytype', None)
        dotposition = request.POST.get('dotposition', None)
        if seriesuid is None or type is None or dotposition is None:
            return Response('参数不完整')
        file_name = time.time()
        dotpositionpath = STATIC_ROOT + '\\dot_element\\' + file_name

        with open(dotpositionpath, 'wb') as f:
            f.write(dotposition)

        data = {
            'seriesuid': seriesuid,
            'overlaytype': overlaytype,
            'dotpositionpath': dotpositionpath
        }

        result = overlay.upload_to_db(**data)

        if result is 'fail':
            return Response('upload overlay fail ！')

        return Response('upload overlay success ！')