# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import os
import platform

import pydicom
from rest_framework.response import Response
from rest_framework.views import APIView

from config.path_cfg import file_path_ferry
from netbase import data_checker
from service import series_svc, patient_svc
from utils.dcm_sorter import DcmSorter
from utils.volume_builder import VolumeBuilder
from netbase.c_log import log


if platform.system() == 'Windows':
    import win32file

    # 如果要部署到linux，更改linux /etc/security/limitd.conf的配置文件，修改最大打开文件数量
    win32file._setmaxstdio(2048)


class Upload(APIView):

    def post(self, request):
        """
        uploading dicom files and parsing information to database
        :param request: a django rest framework request object
        :return: boolean true for success, false for failure
        """
        log.dev_info('uploading...')
        file_name_list = []

        files = request.FILES.getlist('a')
        if len(files) == 0:
            return Response('请选择上传文件')
        log.dev_info('upload {} files!'.format(len(files)))

        for f in files:
            file_name = os.path.join(file_path_ferry.dicomPath, f.name)
            destination = open(file_name, 'wb+')
            for chunk in f.chunks():
                destination.write(chunk)
            destination.close()
            file_name_list.append(file_name)

        dataset_list = []
        series_path_list = []

        log.dev_info('上传完成，正在执行数据入库...')
        for file_name in file_name_list:
            try:
                dataset = pydicom.dcmread(file_name, force=True)
                sorter = DcmSorter()
                seriespath = sorter.split_series(file_name, dataset)
                series_path_list.append(seriespath)
                dataset_list.append(dataset)
                os.remove(file_name)
            except Exception as ex:
                log.dev_error('file name: {}, ex.message: {}'.format(file_name, ex.message))

        patient_svc.upload_dcm(dataset_list, file_path_ferry.splitDicomPath)

        for seriespath in set(series_path_list):
            series_dir = str(seriespath).encode('GB2312')
            log.dev_info('begin data checking:' + series_dir)
            reply = data_checker.DataChecker().data_checking(series_dir)
            if 0 != reply:
                log.dev_error('data checking failed!' + series_dir)
                # return Response('dicom文件不符合规范,创建volume失败')
                continue
            log.dev_info('end data checking:' + series_dir)

            try:
                log.dev_info('begin build volume')
                builder = VolumeBuilder()
                volfilepath, seriesuid = builder.build(seriespath)
            except Exception as ex:
                log.dev_error('build volume failed!' + ex.message)
                # return Response(ex.message)
                continue
            log.dev_info('end build volume')

            try:
                series_svc.upload_volume(volfilepath, seriesuid)
            except Exception as e:
                # return Response(e.message)
                log.dev_error('upload volume failed!' + e.message)
                continue

            log.dev_info('end upload volume:' + seriesuid)

        return Response('success')
