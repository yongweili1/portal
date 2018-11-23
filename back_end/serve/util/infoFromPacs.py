# -*-coding:utf-8-*-
import os

from config.pacs_cfg import pacsconf
from config.path_cfg import file_path_ferry
from serve.md_dicoms.dicom_service import DicomService


class ConnectPacsERROR(Exception):
    pass


class GetInfoFromPacs(object):

    def __init__(self):
        pass

    def connectpacs(self):
        try:
            access_dicom = DicomService(pacs_ae_title=pacsconf.pacs_ae_title, pacs_ip=pacsconf.pacs_ip,
                                        pacs_port=pacsconf.pacs_port,
                                        client_ae_title=pacsconf.client_ae_title, client_port=pacsconf.client_port,
                                        dcm_file_path=pacsconf.dcm_file_path)
            access_dicom.connect()
        except:
            raise ConnectPacsERROR
        return access_dicom

    def getinformations(self, patient_id, patient_name, patient_age, patient_sex, modality):
        patients_list = []
        access_dicom = self.connectpacs()
        patients = access_dicom.find_patients(patient_id=patient_id, patient_name=patient_name, patient_age=patient_age,
                                              patient_sex=patient_sex, modality=modality, date_range="")
        length = len(patients)
        if length != 0:
            for patient in patients:
                patient_dict = {}
                patient_dict['patientName'] = patient.name
                patient_dict['patientId'] = patient.id
                patient_dict['patientAge'] = patient.age
                patient_dict['gender'] = patient.sex
                patient_dict['modality'] = patient.modality
                patients_list.append(patient_dict)
        return patients_list

    def getimage(self, patients_list):

        datasets_list = []
        patient_series_dict = {}

        access_dicom = self.connectpacs()
        access_dicom.set_need_save_file(1)
        series_path = file_path_ferry.splitDicomPath
        access_dicom.set_dcm_file_path(series_path)

        for patientid in patients_list:
            studyuid_list = access_dicom.find_studies_by_patient_id(patientid)

            for studyuid in studyuid_list:
                seriesuid_tuple = access_dicom.find_series_by_study_uid(studyuid)
                seriespath_length_dict = {}

                for seriesuid in seriesuid_tuple[0]:
                    dataset_list = access_dicom.get_series_by_uid(seriesuid)
                    datasets_list.append(dataset_list)
                    series_path = os.path.join(access_dicom.dcm_file_path, str(seriesuid))
                    seriespath_length_dict[len(os.listdir(series_path))] = series_path

                max_series_length = max(seriespath_length_dict.keys())
                seriespath = seriespath_length_dict[max_series_length]
                patient_series_dict[patientid] = seriespath

        return datasets_list, patient_series_dict


pacsinfo = GetInfoFromPacs()
