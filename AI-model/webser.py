#-*-coding:utf-8-*-
#!/usr/bin/python
# coding:utf-8

import sys
import json
from spyne import Application, rpc, ServiceBase
from spyne import Unicode, Iterable
from spyne.protocol.soap import Soap11
from spyne.server.wsgi import WsgiApplication
from wsgiref.simple_server import make_server
import xml.etree.ElementTree as ET

from md.dicom.python.dicom_service import DicomService

from upload_result import UploadResult
import logging

import os
import pandas as pd
from md_ribfracture import load_models, detect_rib_fracture
import xml.etree.ElementTree as ET
import datetime as Time
import csv
import airesult_to_xml as xml
import md.image3d.python.image3d_io as cio


logger = logging.getLogger(__name__)
logger.setLevel(level=logging.INFO)
handler = logging.FileHandler("log.txt")
handler.setLevel(logging.INFO)
formatter = logging.Formatter('%(asctime)s - %(pathname)s - %(filename)s - '
                              '[line:%(lineno)d] - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
logger.addHandler(handler)


def log_enter_and_exit(arg=True):
    def wrapper(func):
        def inner_wrapper(*args, **kwargs):
            logger.info("enter %s" % func.__name__)
            if arg:
                logger.info("arguments:%s, %s" % (args, kwargs))
            else:
                pass
            result = func(*args, **kwargs)
            logger.info("exit %s" % func.__name__)
            return result
        return inner_wrapper
    return wrapper


# def call_dicom_service(self, pacs_ae_title, pacs_ip, pacs_port,client_ae_title, client_port, patientID, accessionNO):
#     client = DicomService(pacs_ae_title, pacs_ip, pacs_port, client_ae_title, client_port)
#     client.connect()
#     patients = client.find_patients(patient_name="", modality="", date_range="", patient_sex="")
#     studies = client.find_studies_by_patient_id_and_accession_number(patientID, accessionNO)
#     series = client.find_series_by_study_uid(studies)
#     images = client.get_images_by_series_uid(series)
#     vol = client.build_volume(images)
#     return vol



reload(sys)
sys.setdefaultencoding('utf8')


class WebService(ServiceBase):

    def __init__(self):
        # self.customer_wsdl = 'http://www.smit-cloud.com:8005/WebAIService.asmx?WSDL'
        pass

    @rpc(Unicode, _returns=Iterable(Unicode))
    def RegisterInfo(self, xml_doc):

        # Get patient's input information from xml
        patient_dic = {'status': '',
                       'patientID': '',
                       'accessionNO': '',
                       'name': '',
                       'birthday': '',
                       'sex': '',
                       'telephone': '',
                       'address': '',
                       'source_id': '',
                       'visitType': '',
                       'visitNumber': '',
                       'bedInfo': '',
                       'studyUID': '',
                       'modality': '',
                       'studyPlace': '',
                       'studyTime': '',
                       'machineInfo': '',
                       'appInfo': '',
                       'clinicalInfo': '',
                       'spareInfo': '',
                       }

        tree = ET.fromstring(xml_doc)
        status = tree.find('STATUS').text
        patient_id = tree.find('PATNO').text
        accession_number = tree.find('ACCNO').text
        name = tree.find('PATNAME').text
        birthday = tree.find('PATBIRTH').text
        sex = tree.find('SEX').text
        telephone = tree.find('TELPHONE').text
        address = tree.find('ADDRESS').text
        source_id = tree.find('PATSOURCEID').text
        visit_type = tree.find('VISITTYPE').text
        visit_number = tree.find('VISITNO').text
        bed_info = tree.find('BEDINFO').text
        study_uid = tree.find('STUUID').text
        modality = tree.find('MODALITY').text
        study_place = tree.find('STUPLACE').text
        study_time = tree.find('STUDATETIME').text
        machine_info = tree.find('MACHINEINFO').text
        app_info = tree.find('APPINFO').text
        clinical_info = tree.find('CLINICALINFO').text
        spare_info = tree.find('SPAREINFO').text

        patient_dic['status'] = status
        patient_dic['patientID'] = patient_id
        patient_dic['accessionNO'] = accession_number
        patient_dic['name'] = name
        patient_dic['birthday'] = birthday
        patient_dic['sex'] = sex
        patient_dic['telephone'] = telephone
        patient_dic['address'] = address
        patient_dic['sourceID'] = source_id
        patient_dic['visitType'] = visit_type
        patient_dic['visitNumber'] = visit_number
        patient_dic['bedInfo'] = bed_info
        patient_dic['studyUID'] = study_uid
        patient_dic['modality'] = modality
        patient_dic['studyPlace'] = study_place
        patient_dic['studyTime'] = study_time
        patient_dic['machineInfo'] = machine_info
        patient_dic['appInfo'] = app_info
        patient_dic['clinicalInfo'] = clinical_info
        patient_dic['spareInfo'] = spare_info

        patient_dic_json = json.dumps(patient_dic, ensure_ascii=False)

        print(patient_dic_json)

        # Connect client AE to PACS
        a = DicomService(pacs_ae_title='UIHPACSSERVER', pacs_ip='10.9.19.175', pacs_port=3333,
                         client_ae_title='205', client_port=3333)
        a.connect()

        # Obtain patient's images at different levels because of the Query/Retrieve Information Model,which
        # includes 'P','S','O','W'

        # Find patient's name and patient's id by args at 'S' model and 'STUDY'
        b = a.find_patients(patient_name="", modality="", date_range="",
                            patient_sex="")
        # for c in b:
        #     print(c)

        # Find patient's studyIUID by patientID at 'S' model and 'STUDY'
        b = a.find_studies_by_patient_id_and_accession_number(patient_dic['patientID'], patient_dic['accessionNO'])
        # print b
        # Find patient's series number by studyIUID at 'S' model and 'SERIES'
        c = a.find_series_by_study_uid(b)

        # Get patient's images by seriesUID at 'S' model and 'IMAGE'
        d = a.get_images_by_series_uid(c)

        e = a.build_volume(d)

        #read volume from local
        # e = cio.read_image(r'/home/lyw/rxl5/pyproject/PKU/mhddata/rxl3.mhd')

        # call AIG ribfracture
        gpu_id = 3
        threshold = 0.8
        model_dir = '/home/lyw/rxl5/boneai/packing_for_xl/models/'
        output_dir = '/home/lyw/rxl5/pyproject/PKU/webresult/'

        logger.info("Load AI Model")
        models = load_models(model_dir, gpu_id)
        logger.info("Load AI Model Done")
        logger.info("Start cacluate")
        df_fracture = detect_rib_fracture('web_result', e, models, threshold)
        logger.info("Cacluate done")
        logger.info("Save results file")
        df_fracture.to_csv(os.path.join(output_dir, 'results.csv'))


        # call segmentation AI
        # model = autoseg_load_model('E:\AlgModel\spleen', 0)
        #
        # seg, _, _, _ = autoseg_volume(e, model)
        #
        # write_image(e, r"E:\images\volume.nii.gz")
        #
        # write_image(seg, r"E:\images\test.nii.gz")


        # transform AIresult_csv to xml
        logger.info("translate csv to xml")
        un_csv = xml.analy_csv(r'/home/lyw/rxl5/pyproject/PKU/webresult/results.csv')
        xml.ai_to_xml(un_csv, r'/home/lyw/rxl5/pyproject/PKU/webresult/a1.xml')



        # upload result
        # result = 'qwertyuio'
        result_file = open(r'/home/lyw/rxl5/pyproject/PKU/webresult/a1.xml')
        result = result_file.read()
        logger.info("Start upload result")
        ur = UploadResult()
        final = ur.send_result(result)
        logger.info("Upload result succeed")
        print final

    @rpc(Unicode, _returns=Iterable(Unicode))
    def ReportInfo(self, xml_doc):
        pass

    @rpc(Unicode, _returns=Iterable(Unicode))
    def SRAIInfo(self, xml_doc):
        pass


application = Application([WebService], 'spyne.examples.hello',
                          in_protocol=Soap11(validator='lxml'), out_protocol=Soap11())
wsgi_application = WsgiApplication(application)


if __name__ == '__main__':
    import logging

    host = '127.0.0.1'
    port = 8000

    logging.info("listening to http://127.0.0.1:8000")
    logging.info("wsdl is at: http://localhost:8000/?wsdl")

    server = make_server(host, port, wsgi_application)
    server.serve_forever()
