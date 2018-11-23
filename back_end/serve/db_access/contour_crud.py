import os
import json

from serve.db_access.models import Contour
from serve.util.generate_uid import GenerateUid
from serve.db_access.serializer import ContourSerializer


class ContourCrud(object):
    def __init__(self):
        self.filepath = r'D:\volume'
        pass

    def Delete(self, image_uid, roi_uid):
        try:
            Contour.objects.filter(imageuid=image_uid, roiuid=roi_uid).delete()
        except Exception as ex:
            print ex.message
            return 1
        return 0

    def Insert(self, image_uid, roi_uid, contour):
        contour_uid = GenerateUid.contour_uid()
        file_name = contour_uid
        cpspath = os.path.join(self.filepath, file_name)
        with open(cpspath, 'wb') as f:
            f.write(json.dumps(contour))

        params = {
            'roiuid': roi_uid,
            'contouruid': contour_uid,
            'cpspath': cpspath,
            'imageuid': image_uid
        }
        try:
            contour = ContourSerializer(data=params)
            contour.is_valid(raise_exception=True)
            contour.save()
        except Exception as ex:
            print ex.message
            return 1
        return 0

    def Retrieve(self, image_uid, roi_uid):
        cps = None
        try:
            cps = Contour.objects.filter(imageuid=image_uid, roiuid=roi_uid)
        except Exception as ex:
            print ex.message
        return cps

    def Retrieve(self, image_uid):
        cps = None
        try:
            cps = Contour.objects.filter(imageuid=image_uid)
        except Exception as ex:
            print ex.message
        return cps
