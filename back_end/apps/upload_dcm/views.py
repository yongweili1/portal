# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import os
import platform
import pydicom

from rest_framework.response import Response
from rest_framework.views import APIView

from serve.static_parameters.setFilePath import filepath
from serve.util.splitDicom import SplitDicoms
from serve.DBAccess.upload_dcm_to_db import UploadDcm
from serve.DBAccess.upload_vol_to_db import UploadVolume
from serve.util.buildVolume import DicomToVolume

if platform.system() == 'Windows':
    import win32file

    win32file._setmaxstdio(2048)  # 如果要部署到linux，更改linux /etc/security/limitd.conf的配置文件，修改最大打开文件数量


class Patinfo(APIView):

    def post(self, request):
        """
        uploading dicom files and parsing information to database
        :param request: a django rest framework request object
        :return: boolean true for success, false for failure
        """
        print('正在上传，loading...')
        file_name_list = []

        files = request.FILES.getlist('a')
        if len(files) == 0:
            return Response('请选择上传文件')
        print(len(files))

        for f in files:
            destination = open(filepath.dicomPath + f.name, 'wb+')
            for chunk in f.chunks():
                destination.write(chunk)
            destination.close()
            file_name_list.append(f.name)

        dataset_list = []
        series_path_list = []

        print('上传完成，正在执行DCM数据入库...')
        for file_name in file_name_list:
            # if platform.system() == 'Windows':
            file_path = os.path.join(filepath.dicomPath, file_name)
            dataset = pydicom.dcmread(file_path, force=True)
            splitdicoms = SplitDicoms()
            try:
                seriespath = splitdicoms.split_series(file_name, dataset)
                series_path_list.append(seriespath)
            except Exception as e:
                print('分离DCM失败')
            dataset_list.append(dataset)

        # try:
        uploaddcm = UploadDcm()
        uploaddcm.upload_dcm(dataset_list)
        # except Exception as e:
        #     return Response('DCM数据入库失败，请检查DCM数据是否符合DB字段约束')

        print('数据入库成功，重新build_volume（此操作比较耗时，请稍等）...')
        for seriespath in set(series_path_list):
            if len(os.listdir(seriespath)) <= 1:
                return Response('series下的dicom文件单一，无法build volume')
            try:
                buildvol = DicomToVolume()
                volfilepath, seriesuid = buildvol.dicom_to_volume(seriespath)
            except Exception as e:
                return Response('dicom文件不符合规范,创建volume失败')
            try:
                UploadVolume(volfilepath, seriesuid)
            except Exception as e:
                return Response('Volume入库失败')

        return Response('success')
