# -*-coding:utf-8-*-
from django.db import transaction

from back_end.util.readDcm import DcmSeries, DcmImage
from patientinformations.models import Series, Image


class DBError(Exception):
    pass


def UploadVolume(volfilepath, datasetlist):

    dcmseries = DcmSeries()
    series_dic = dcmseries.get_dicom_series(datasetlist[0])
    series_data = {
        'seriespixeldatafilepath': volfilepath,
        'buildvolumesign': int(0)
    }

    try:
        Series.objects.filter(seriesuid=series_dic['seriesuid']).update(**series_data)
    except Exception as e:
        raise DBError
