# -*- coding: utf-8 -*-
from __future__ import unicode_literals

# Create your views here.
from rest_framework.generics import GenericAPIView
from rest_framework.response import Response
from md.dicom.python.dicom_service import DicomService
from handle_pacs.setPacs import PacsConf
from django.http import HttpResponse
from md.dicom.python.dicom_service import DicomService
from rest_framework.views import APIView

from back_end.util.upload_dcm_to_db import UploadDcm


class GetPacs(APIView):

    def get(self, request):
        pacs_ae_title = str(request.GET.get('pacs_ae_title')[1:-1])
        pacs_ip = str(request.GET.get('pacs_ip')[1:-1])
        pacs_port = int(request.GET.get('pacs_port'))

        client_ae_title = str(request.GET.get('client_ae_title')[1:-1])
        client_port = int(request.GET.get('client_port'))

        # a = DicomService(pacs_ae_title=pacs_ae_title, pacs_ip=pacs_ip, pacs_port=pacs_port,
        #                  client_ae_title=client_ae_title, client_port=client_port)
        # a.connect()
        #
        # b = a.find_studies_by_patient_id_and_accession_number('R1107693', '10306916')
        # print b

        response = Response("ok")

        return response


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
