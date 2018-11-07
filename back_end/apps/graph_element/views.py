# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import time
import json

# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView
from back_end.settings import STATIC_ROOT
from serve.DBAccess.upload_dot_to_db import contour
from serve.util.connectImageServe import screen2world


class GraphElement(APIView):

    def post(self, request):
        """
        upload overlay to local file and save path to DB
        :param seriesuid:the id of volume belong to overlay
        :param overlaytype: shape type of overlay
        :param dotpositon: point coordinate
        :return:
        """
        series_uid = request.data.get('series_uid', None)
        cps = request.data.get('cps', None)
        user_ip = request.META.get('REMOTE_ADDR', None)
        params = {
            'trans_direct_flag': 'screen2world',
            'focus_view': 'transverse',
            'point_2d': [cps[0]['x'], cps[0]['y']],
            'user_ip': user_ip,
            'server_name': 'image',
            'command': 'word_coord'
        }
        rst = screen2world(**params)
        point3d = json.loads(rst.kwargs)
        print(point3d)
        patientposition_z = float(point3d[2])

        cps_world = []
        for cp in cps:
            params['point_2d'] = [cp['x'], cp['y']]
            rst = screen2world(**params)
            point3d = json.loads(rst.kwargs)
            cps_world.append(point3d)

        file_name = user_ip + '-' + str(time.time())
        cpspath = r'D:\svr\volume' + '\\' + file_name + '.txt'

        with open(cpspath, 'wb') as f:
            f.write(json.dumps(cps_world))

        data = {
            'seriesuid': series_uid,
            'patientposition_z': patientposition_z,
            'cpspath': cpspath
        }
        try:
            result = contour.upload_to_db(**data)
        except Exception as e:
            print(e.message)

        if result is 'fail':
            return Response('upload overlay fail ！')

        return Response('upload overlay success ！')
