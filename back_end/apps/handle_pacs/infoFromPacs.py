# -*-coding:utf-8-*-
from setPacs import pacsconf
# from md.dicom.python.dicom_service import DicomService
from pku.dicom.dicom_service import DicomService
# from back_end.util.dicom_service import DicomService


class ConnectPacsERROR(Exception):
    pass


class GetInfoFromPacs(object):

    def __init__(self):
        self.patients_list = []

    def connectpacs(self):
        try:
            access_dicom = DicomService(pacs_ae_title=pacsconf.pacs_ae_title, pacs_ip=pacsconf.pacs_ip,
                                        pacs_port=pacsconf.pacs_port,
                                        client_ae_title=pacsconf.client_ae_title, client_port=pacsconf.client_port,
                                        dcm_file_path='d:/dcm')
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


pacsinfo = GetInfoFromPacs()
