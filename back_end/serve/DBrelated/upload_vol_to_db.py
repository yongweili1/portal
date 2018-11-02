# -*-coding:utf-8-*-
from django.db import transaction

from serve.util.models import Series, Image


class DBError(Exception):
    pass


def UploadVolume(volfilepath, seriesuid):

    series_data = {
        'seriespixeldatafilepath': volfilepath,
        'buildvolumesign': int(0)
    }

    try:
        Series.objects.filter(seriesuid=seriesuid).update(**series_data)
    except Exception as e:
        raise DBError
