# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import os
import platform

import pydicom

from rest_framework.response import Response
from rest_framework.views import APIView

from config.path_cfg import file_path_ferry
from util.dcm_sorter import DcmSorter
from db_access.upload_dcm_to_db import UploadDcm
from db_access.upload_vol_to_db import UploadVolume
from util.volume_builder import VolumeBuilder
from netbase import data_checker

if platform.system() == 'Windows':
    import win32file

    # 如果要部署到linux，更改linux /etc/security/limitd.conf的配置文件，修改最大打开文件数量
    win32file._setmaxstdio(2048)


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
            file_name = os.path.join(file_path_ferry.dicomPath, f.name)
            destination = open(file_name, 'wb+')
            for chunk in f.chunks():
                destination.write(chunk)
            destination.close()
            file_name_list.append(file_name)

        dataset_list = []
        series_path_list = []

        print('上传完成，正在执行数据入库...')
        for file_name in file_name_list:
            try:
                dataset = pydicom.dcmread(file_name, force=True)
                sorter = DcmSorter()
                seriespath = sorter.split_series(file_name, dataset)
                series_path_list.append(seriespath)
                dataset_list.append(dataset)
                os.remove(file_name)
            except Exception as ex:
                print '文件解析失败', file_name, ex.message

        uploaddcm = UploadDcm()
        uploaddcm.upload_dcm(dataset_list, file_path_ferry.splitDicomPath)

        for seriespath in set(series_path_list):
            try:
                if len(os.listdir(seriespath)) <= 1:
                    print ('series下的dicom文件单一，无法build volume')
                    continue

                if platform.system() == 'Windows':
                    print('begin data checking')
                    series_dir = str(seriespath).encode('GB2312')
                    reply = data_checker.DataChecker().data_checking(series_dir)
                    if '' != reply:
                        print reply
                        return Response('dicom文件不符合规范,创建volume失败')

                print('begin build volume')
                builder = VolumeBuilder()
                volfilepath, seriesuid = builder.build(seriespath)
            except Exception as ex:
                print ex.message
                return Response('创建volume异常')

            try:
                UploadVolume(volfilepath, seriesuid)
            except Exception as e:
                return Response('Volume入库失败')

        return Response('success')

