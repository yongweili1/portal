# -*-coding:utf-8-*-
from django.db import transaction

from back_end.util.readDcm import DcmSeries, DcmImage
from patientinformations.models import Series, Image


class DBError(Exception):
    pass


def UploadVolume(volfilepath, datasetlist):

    dcmseries = DcmSeries()
    series_dic = dcmseries.get_dicom_series(datasetlist[0])
    dcmimage = DcmImage()
    imageuid_list  = []
    for dataset in datasetlist:
        imageuid = dcmimage.get_dicom_image(dataset)
        imageuid_list.append(imageuid)

    series_data = {
        'seriespixeldatafilepath': volfilepath,
        'buildvolumesign': int(0)
    }

    image_data = {
        'updatesign': int(1)
    }

    # 开启事务
    with transaction.atomic():
        # 创建保存点，用于提交和回滚
        save_id = transaction.savepoint()

        try:
            Series.objects.filter(seriesuid=series_dic['seriesuid']).update(**series_data)
            for imgid in imageuid_list:
                Image.objects.filter(imageuid=imgid).update(**image_data)
        except Exception as e:
            transaction.savepoint_rollback(save_id)
            raise DBError
        else:
            transaction.savepoint_commit(save_id)