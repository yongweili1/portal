# -*- coding: utf-8 -*-
from __future__ import unicode_literals

# Create your views here.
import math
import os

from rest_framework.response import Response
from rest_framework.views import APIView

from serve.util.infoFromPacs import pacsinfo
from serve.util.infoFromPacs import ConnectPacsERROR
from serve.DBAccess.upload_dcm_to_db import UploadDcm
from serve.DBAccess.upload_vol_to_db import UploadVolume
from serve.util.buildVolume import DicomToVolume


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
        :param page:current page
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
        if patient_sex == "Male":
            patient_sex = "M"
        elif patient_sex == "Female":
            patient_sex = "F"
        else:
            patient_sex = ""
        if modality == "undefined":
            modality = ""

        if (len(SavePatient.patients_list) == 0) or \
                (patient_id != SavePatient.patient_id or
                 patient_name != SavePatient.patient_name or
                 patient_age != SavePatient.patient_age or
                 patient_sex != SavePatient.patient_sex or
                 modality != SavePatient.modality):
            try:
                SavePatient.patients_list = pacsinfo.getinformations(patientName=patient_name, patientSex=patient_sex, modality=modality)
                SavePatient.patient_id = patient_id
                SavePatient.patient_name = patient_name
                SavePatient.patient_age = patient_age
                SavePatient.patient_sex = patient_sex
                print("重新下载")

            except ConnectPacsERROR as e:
                return Response('PACS连接失败')
        else:
            print("不下载")

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


class DownloadSeries(APIView):

    def get(self, request):
        patients_unicode = request.GET.get('patients', None)
        patients_str = patients_unicode.encode('utf-8')
        if not patients_str:
            return Response('请传入有效的patientID')
        patients_list = patients_str.split(',')
        try:
            datasets_list, patient_series_dict = pacsinfo.getimage(patients_list)
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
                buildvol = DicomToVolume()
                volfilepath, seriesuid = buildvol.dicom_to_volume(seriespath)
            except Exception as e:
                return Response('dicom文件不符合规范,创建volume失败')
            try:
                UploadVolume(volfilepath, seriesuid)
            except Exception as e:
                return Response('Volume入库失败')

        return Response('Download Success')


# class SavePacsImageByPatient(APIView):
#     """
#     input: a patient id list
#     save images to database
#     """
#
#     def get(self, request):
#         try:
#             patient_id_str = str(request.GET.get('patient_id_list'))
#             patient_id_list = []
#             if ',' not in patient_id_str:
#                 patient_id_list.append(patient_id_str)
#             else:
#                 patient_id_all = patient_id_str.split(',')
#                 patient_id_list = map(str, patient_id_all)
#         except Exception as e:
#             return Response('patient id is not valid')
#
#         try:
#             access_dicom = pacsinfo.connectpacs()
#             access_dicom.set_need_save_file(1)
#
#             series_path = SaveDicomFilePath.location_3
#             access_dicom.set_dcm_file_path(series_path)
#             for patient_id in patient_id_list:
#                 patient_studies = access_dicom.find_studies_by_patient_id(patient_id)
#                 for patient_study in patient_studies:
#                     patient_series, _ = access_dicom.find_series_by_study_uid(patient_study)
#                     # patient_series = access_dicom.find_series_by_study_uid(patient_study)
#                     for series_uid in patient_series:
#                         dataset_list = access_dicom.get_series_by_uid(series_uid)
#                         upload_dcm = UploadDcm()
#                         upload_dcm.upload_dcm(datasetlist=dataset_list)
#                         # pool = threadpool.ThreadPool(10)
#                         # requests = threadpool.makeRequests(upload_dcm.upload_dcm, dataset_list)
#                         # [pool.putRequest(req) for req in requests]
#         except Exception as e:
#             return Response('download failed')
#
#         return Response('OK')
