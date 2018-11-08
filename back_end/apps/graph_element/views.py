# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import time
import json

# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView
from back_end.settings import STATIC_ROOT
from serve.DBAccess.models import Series
from serve.DBAccess.upload_dot_to_db import contour
from serve.util.connectImageServe import request


class GraphElement(APIView):
    def get(self, request):
        roi_uid = request.GET.get('roi_uid', None)
        cross_point = request.GET.get('cross_point', None)
        point3d = self.get_point3d(cross_point)
        z = float(point3d[2])
        contours = Contours.objects.filter(roi_uid=roi_uid).filter(patientposition_z=z)
        contour_cps = []

        if contours is not None and len(contours) > 0:
            for item in contours:
                path = item['cpspath']
                point2ds = []
                with open(path, 'rb') as f:
                    cps = json.loads(f.read())
                    for cp in cps:
                        point2ds.append(self.get_point2d(cp))
                contour_cps.append(point2ds)

        return Response(contour_cps)

    def post(self, request):
        """
        upload contour to local file and save path to DB
        :param uid: primary key
        :param roi_uid: related roi's uid
        :param cps: point sets
        :return: True or False
        """
        uid = request.data.get('uid', None)
        roi_uid = request.data.get('roi_uid', None)
        cps = request.data.get('cps', None)
        user_ip = request.META.get('REMOTE_ADDR', None)

        # get z index of current slice
        point3d = self.get_point3d([cps[0]['x'], cps[0]['y']])
        patientposition_z = float(point3d[2])

        # converter screen point to patient point
        cps_world = []
        for cp in cps:
            point3d = self.get_point3d([cp['x'], cp['y']])
            cps_world.append(point3d)

        # if uid is null, add a new record, otherwise, update it
        if uid is None:
            file_name = user_ip + '-' + str(time.time())
            cpspath = r'D:\svr\volume' + '\\' + file_name + '.txt'
            self.save_cps_to_file(cps_world, cpspath)
            data = {
                'roi_uid': roi_uid,
                'patientposition_z': patientposition_z,
                'cpspath': cpspath
            }
            result = contour.upload_to_db(**data)
            return Response('Save contour success！')
        else:
            contours = Contours.objects.filter(uid=uid)\
                                       .filter(patientposition_z=float(patientposition_z))\
                                       .filter(roi_uid=roi_uid)
            if len(contours) == 1:
                cpspath = contours[0]['cpspath']
                self.save_cps_to_file(cps_world, cpspath)
                # update this record?
                return Response('Update exist contour succeed.')
                pass
            else:
                return Response('Failed to update exist contour.')

    def get_point3d(self, point2d):
        params = {
            'point_2d': point2d,
            'command': 'point3d'
        }
        rst = request(**params)
        point3d = json.loads(rst.kwargs)
        return point3d

    def get_point2d(self, point3d):
        params = {
            'point_3d': point3d,
            'command': 'point3d'
        }
        rst = request(**params)
        point2d = json.loads(rst.kwargs)
        return point2d

    def save_cps_to_file(self, cps, path):
        with open(path, 'wb') as f:
            f.write(json.dumps(cps))
