# -*-coding:utf-8-*-

from serve.db_access.models import Series


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
