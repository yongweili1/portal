# -*- coding: utf-8 -*-
from __future__ import unicode_literals

# Create your views here.
import json
import math
import threadpool
from rest_framework.response import Response

from rest_framework.views import APIView
from infoFromPacs import pacsinfo
from infoFromPacs import ConnectPacsERROR

from back_end.util.upload_dcm_to_db import UploadDcm


class GetPatient(APIView):

    def get(self, request):

        size = int(request.GET.get('size', 15))
        page = int(request.GET.get('page', 0))

        try:
            patients_list = pacsinfo.getinformations()
        except ConnectPacsERROR as e:
            return Response(e)
        totalelements = len(patients_list)
        if totalelements == 0:
            return Response('PACS服务器无数据,请检查PACS')

        totalpages = int(math.ceil(float(totalelements) / float(size)))
        totalpatients = patients_list[size * page:size * (page + 1)]
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


class GetPacsSeries(APIView):
    """
    input: patient_id, accession_number
    output: patient_series
    """

    def get(self, request):
        patient_id = str(request.GET.get('patient_id'))
        accession_number = str(request.GET.get('accession_number'))
        # pacs = PacsConf()
        # access_dicom = DicomService(pacs_ae_title=pacs.pacs_ae_title, pacs_ip=pacs.pacs_ip, pacs_port=pacs.pacs_port,
        #                             client_ae_title=pacs.client_ae_title, client_port=pacs.client_port)
        # access_dicom.connect()
        #
        # patient_studies = access_dicom.find_studies_by_patient_id_and_accession_number(patient_id, accession_number)
        # patient_series = access_dicom.find_series_by_study_uid(patient_studies)
        # print patient_studies
        # print patient_series
        #
        # return Response(patient_series)

        # b = a.find_studies_by_patient_id_and_accession_number('R1107693', '10306916')
        # print b


class SavePacsImage(APIView):
    """
    input: series_uid
    save images to database
    """

    def get(self, request):
        seriesuid = str(request.GET.get('series_uid'))

        # pacs = PacsConf()
        # access_dicom = DicomService(pacs_ae_title=pacs.pacs_ae_title, pacs_ip=pacs.pacs_ip, pacs_port=pacs.pacs_port,
        #                             client_ae_title=pacs.client_ae_title, client_port=pacs.client_port)
        # access_dicom.connect()
        # datasetlist = access_dicom.get_images_by_series_uid(seriesuid)
        #
        # upload_dcm = UploadDcm()
        # upload_dcm.upload_dcm(datasetlist=datasetlist)

        return Response('OK')


class SavePacsImageByPatient(APIView):
    """
    input: a patient id list
    save images to database
    """

    def get(self, request):
        try:
            patient_id_str = str(request.GET.get('patient_id_list'))
            patient_id_list = []
            if ',' not in patient_id_str:
                patient_id_list.append(patient_id_str)
            else:
                patient_id_all = patient_id_str.split(',')
                patient_id_list = map(str, patient_id_all)
        except Exception as e:
            return Response('patient id is not valid')

        try:
            access_dicom = pacsinfo.connectpacs()
            for patient_id in patient_id_list:
                patient_studies = access_dicom.find_studies_by_patient_id(patient_id)
                for patient_study in patient_studies:
                    patient_series = access_dicom.find_series_by_study_uid(patient_study)
                    for series_uid in patient_series:
                        dataset_list = access_dicom.get_images_by_series_uid(series_uid)
                        upload_dcm = UploadDcm()
                        # upload_dcm.upload_dcm(datasetlist=dataset_list)
                        pool = threadpool.ThreadPool(10)
                        requests = threadpool.makeRequests(upload_dcm.upload_dcm, dataset_list)
                        [pool.putRequest(req) for req in requests]
        except Exception as e:
            return Response('download failed')

        return Response('OK')
