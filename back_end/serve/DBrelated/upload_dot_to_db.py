# -*-coding:utf-8-*-
from serve.util.models import Contours


class Contour(object):
    def upload_to_db(self, **kwargs):
        try:
            Contours.objects.create(**kwargs)
        except Exception as e:
            print(e.message)
            return 'fail'
        return 'success'


contour = Contour()