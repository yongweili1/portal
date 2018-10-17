# -*-coding:utf-8-*-
from models import GraphElement


class OverLay(object):
    def upload_to_db(self, **kwargs):
        try:
            GraphElement.objects.create(**kwargs)
        except:
            return 'fail'
        return 'success'


overlay = OverLay()
