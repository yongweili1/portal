# -*-coding:utf-8-*-
from serve.DBAccess.models import Script


class UploadScript(object):
    def uploadscript(self, *args, **kwargs):
        try:
            Script.objects.create(**kwargs)
        except:
            return 'fail'
        return 'success'


script = UploadScript()