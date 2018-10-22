# -*- coding: utf-8 -*-
from __future__ import unicode_literals
import os
import pydicom
# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView

from back_end.util.setFilePath import SaveDicomFilePath

import win32file

from upload_dcm.split_dicoms import SplitDicoms
from back_end.util.upload_dcm_to_db import UploadDcm

from back_end.util.buildVolume import DicomToVolume
from back_end.util.upload_vol_to_db import UploadVolume

win32file._setmaxstdio(2048)  # 如果要部署到linux，则需要更改linux  /etc/security/limitd.conf的配置文件，修改最大打开文件数量


class Patinfo(APIView):

    def post(self, request):
        """
        uploading dicom files and parsing information to database
        :param request: a django rest framework request object
        :return: boolean true for success, false for failure
        """

        file_name_list = []

        files = request.FILES.getlist('a')
        if len(files) == 0:
            return Response('请选择上传文件')

        # 将上传的文件存到本地
        for file in files:
            destination = open(SaveDicomFilePath.location_2 + file.name, 'wb+')
            for chunk in file.chunks():
                destination.write(chunk)
            destination.close()
            file_name_list.append(file.name)

        # 读取，解析保存的文件
        dataset_list = []
        series_path_list = []

        for file_name in file_name_list:
            file_path = os.path.join(SaveDicomFilePath.location_2, file_name)
            dataset = pydicom.dcmread(file_path, force=True)
            splitdicoms = SplitDicoms()
            seriespath = splitdicoms.split_patient(file_name, dataset)
            dataset_list.append(dataset)
            series_path_list.append(seriespath)

        try:
            uploaddcm = UploadDcm()
            uploaddcm.upload_dcm(dataset_list)
        except Exception as e:
            return Response('DCM数据入库异常')

        for seriespath in set(series_path_list):
            filelist = os.listdir(seriespath)
            datasetlist = []
            for filename in filelist:
                filepath = os.path.join(seriespath, filename)
                dataset = pydicom.dcmread(filepath, force=True)
                datasetlist.append(dataset)
            try:
                buildvol = DicomToVolume()
                volfilepath = buildvol.dicom_to_volume(datasetlist)
            except ValueError:
                return Response('dicom文件不完整,创建volume失败')
            try:
                UploadVolume(volfilepath, datasetlist)
            except Exception as e:
                return Response('Volume入库异常')

        return Response('success')
