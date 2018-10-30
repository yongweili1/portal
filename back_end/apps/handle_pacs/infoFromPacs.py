# -*-coding:utf-8-*-
import os

from setPacs import pacsconf
from md.dicom.python.dicom_service import DicomService
from md.dicom.python.c_get_scu import CGetScu
from back_end.util.setFilePath import SaveDicomFilePath
from pydicom.dataset import Dataset


class ConnectPacsERROR(Exception):
    pass


class GetInfoFromPacs(object):

    def __init__(self):
        self.patients_list = []

    def connectpacs(self):
        try:
            access_dicom = DicomService(pacs_ae_title=pacsconf.pacs_ae_title, pacs_ip=pacsconf.pacs_ip,
                                        pacs_port=pacsconf.pacs_port,
                                        client_ae_title=pacsconf.client_ae_title, client_port=pacsconf.client_port)

            access_dicom.connect()
        except:
            raise ConnectPacsERROR('PACS连接失败')
        return access_dicom

    def getinformations(self):
        access_dicom = self.connectpacs()
        patients = access_dicom.find_patients(patient_name="", modality="", date_range="", patient_sex="")
        length = len(patients)
        if length != 0:
            for patient in patients:
                patient_dict = {}
                patient_dict['patientName'] = patient.name
                patient_dict['patientId'] = patient.id
                self.patients_list.append(patient_dict)
        return self.patients_list

    def getimage(self, patients_list):

        datasets_list = []
        patient_series_dict = {}

        access_dicom = self.connectpacs()
        access_dicom.set_need_save_file(1)

        for patientid in patients_list:
            studyuid_list = access_dicom.find_studies_by_patient_id(patientid)

            for studyuid in studyuid_list:
                seriesuid_list = access_dicom.find_series_by_study_uid(studyuid)
                seriespath_length_dict = {}

                for seriesuid in seriesuid_list:
                    dataset_list = access_dicom.get_images_by_series_uid(seriesuid)
                    datasets_list.append(dataset_list)
                    series_path = os.path.join(access_dicom.dcm_file_path, str(seriesuid))
                    seriespath_length_dict[len(os.listdir(series_path))] = series_path

                max_series_length = max(seriespath_length_dict.keys())
                seriespath = seriespath_length_dict[max_series_length]
                patient_series_dict[patientid] = seriespath

        return datasets_list, patient_series_dict


pacsinfo = GetInfoFromPacs()
