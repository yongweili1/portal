# -*- coding: utf-8 -*-
from __future__ import unicode_literals

# Create your views here.
import math
import os

from rest_framework.response import Response
from rest_framework.views import APIView

from utils.pacs_connector import pacs_conn, PacsConnectError
from db_context.upload_dcm_to_db import UploadDcm
from db_context.upload_vol_to_db import UploadVolume
from utils.volume_builder import VolumeBuilder


class SavePatient():
    patients_list = []
    patient_id = ""
    patient_name = ""
    patient_age = ""
    patient_sex = ""
    modality = ""


class GetPatient(APIView):

    def get(self, request):
        """
        provide patients information from PACS
        :param size:the number of data on the current page
        :param page:current page--0
        :return: information list
        """
        size = int(request.GET.get('size', 15))
        page = int(request.GET.get('page', 0))
        patient_id = request.GET.get('patientId', "")
        patient_name = request.GET.get('patientName', "")
        patient_age = request.GET.get('patientAge', "")
        patient_sex = request.GET.get('gender', "")
        modality = request.GET.get('modality', "")

        if patient_id == "undefined":
            patient_id = ""
        if patient_name == "undefined":
            patient_name = ""
        if patient_age == "undefined":
            patient_age = ""

        if patient_sex == "undefined":
            patient_sex = ""
        elif len(patient_sex) > 10:
            patient_sex = patient_sex[10]
            if patient_sex != "M" and patient_sex != "F":
                patient_sex = ""
        else:
            patient_sex = ""

        if modality == "undefined":
            modality = ""
        elif len(modality) > 10:
            modality = modality[10: len(modality)-2]
            if modality == "All":
                modality = ""
        else:
            modality = ""

        try:
            patients_list = pacs_conn.getinformations(patient_id=patient_id, patient_name=patient_name,
                                                      patient_age=patient_age,
                                                      patient_sex=patient_sex, modality=modality)
            SavePatient.patients_list = patients_list
            SavePatient.patient_id = patient_id
            SavePatient.patient_name = patient_name
            SavePatient.patient_age = patient_age
            SavePatient.patient_sex = patient_sex
            SavePatient.modality = modality

        except PacsConnectError as e:
            return Response('PACS连接失败')

        totalelements = len(SavePatient.patients_list)
        if totalelements == 0:
            return Response('PACS服务器无数据,请检查PACS')

        totalpages = int(math.ceil(float(totalelements) / float(size)))
        totalpatients = SavePatient.patients_list[size * page:size * (page + 1)]
        numberofelements = len(totalpatients)

        data = {
            'content': totalpatients,
            'totalPages': totalpages,
            'totalElements': totalelements,
            'size': size,
            'number': page,
            'numberOfElements': numberofelements
        }

        return Response(data)


class GetPacsPage(APIView):

    def get(self, request):
        """
        provide patients information from Local memory
        :param size:the number of data on the current page
        :param page:current page
        :return: information list
        """
        size = int(request.GET.get('size', 15))
        page = int(request.GET.get('page', 0))

        totalelements = len(SavePatient.patients_list)
        totalpages = int(math.ceil(float(totalelements) / float(size)))
        totalpatients = SavePatient.patients_list[size * page:size * (page + 1)]
        numberofelements = len(totalpatients)

        data = {
            'content': totalpatients,
            'totalPages': totalpages,
            'totalElements': totalelements,
            'size': size,
            'number': page,
            'numberOfElements': numberofelements
        }

        return Response(data)


class DownloadSeries(APIView):

    def get(self, request):
        patients_unicode = request.GET.get('patientId', None)
        patients_str = patients_unicode.encode('utf-8')
        print patients_str
        if not patients_str:
            return Response('请传入有效的patientID')
        patients_list = patients_str.split(',')
        try:
            datasets_list, patient_series_dict = pacs_conn.getimage(patients_list)
        except Exception as e:
            return Response('从PACS获取数据过程出错，请重试')

        for dataset_list in datasets_list:
            try:
                uploaddcm = UploadDcm()
                uploaddcm.upload_dcm(dataset_list)
            except Exception as e:
                return Response('DCM数据入库失败，请检查DCM数据是否符合DB字段约束')

        for seriespath in patient_series_dict.values():
            if len(os.listdir(seriespath)) <= 1:
                return Response('series下的dicom文件单一，无法build volume')
            try:
                builder = VolumeBuilder()
                volfilepath, seriesuid = builder.build(seriespath)
            except Exception as e:
                return Response('dicom文件不符合规范,创建volume失败')
            try:
                UploadVolume(volfilepath, seriesuid)
            except Exception as e:
                return Response('Volume入库失败')

        return Response('Download Success')
