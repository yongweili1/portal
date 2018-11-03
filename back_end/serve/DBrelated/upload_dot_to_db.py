# -*-coding:utf-8-*-
from serve.util.models import Contours


class OverLay(object):
    def upload_to_db(self, **kwargs):
        try:
            Contours.objects.create(**kwargs)
        except:
            return 'fail'
        return 'success'


overlay = OverLay()
