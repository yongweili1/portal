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
import ConfigParser



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


def analyze_xml(xml_doc):
    """
    extract the information in xml files that come from the client
    :param xml_doc: xml file
    :return: dict that contain patient information
    """
    xml_doc = f
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
    return patient_dic_json



@log_enter_and_exit()
def call_dicom_service(pacs_ae_title, pacs_ip, pacs_port,client_ae_title, client_port, patient_id, accession_no):
    """
    connect the PACS, get the DICOM files and transform it to image3D volume
    :param pacs_ae_title:
    :param pacs_ip:
    :param pacs_port:
    :param client_ae_title:
    :param client_port:
    :param patient_id:
    :param accession_no:
    :return:
    """
    client = DicomService(pacs_ae_title, pacs_ip, pacs_port, client_ae_title, client_port)
    client.connect()
    studies = client.find_studies_by_patient_id_and_accession_number(patient_id, accession_no)
    series = client.find_series_by_study_uid(studies)
    images = client.get_images_by_series_uid(series)
    vol = client.build_volume(images)
    return vol


@log_enter_and_exit(arg=False)
def call_ai_model(image3d, gpu_id, threshold):
    """
    Load AI model and save the AI result
    :param image3d: image3d volume
    :param gpu_id: the GPU number
    :param threshold: if algorithm need
    :return:
    """
    model_dir = '/home/lyw/rxl5/boneai/packing_for_xl/models/'
    output_dir = '/home/lyw/rxl5/pyproject/PKU/webresult/'
    logger.info("Load AI Model")
    models = load_models(model_dir, gpu_id)
    logger.info("Load AI Model succeed!")
    logger.info("Start cacluate")
    df_fracture = detect_rib_fracture('web_result', image3d, models, threshold)
    logger.info("Calculate done")
    logger.info("Save results file")
    df_fracture.to_csv(os.path.join(output_dir, 'results.csv'))


@log_enter_and_exit()
def ai_result_to_xml(dir):
    """
    transform the csv file to xml file
    :param dir: the AI result csv file directory
    :return:
    """
    logger.info("translate csv to xml")
    un_csv = xml.analy_csv(r'/home/lyw/rxl5/pyproject/PKU/webresult/results.csv')
    xml.ai_to_xml(un_csv, r'/home/lyw/rxl5/pyproject/PKU/webresult/a1.xml')



reload(sys)
sys.setdefaultencoding('utf8')


class WebService(ServiceBase):

    def __init__(self):
        # self.customer_wsdl = 'http://www.smit-cloud.com:8005/WebAIService.asmx?WSDL'
        pass

    @rpc(Unicode, _returns=Iterable(Unicode))
    def RegisterInfo(self, xml_doc):

        dic = analyze_xml(xml_doc)
        print dic['patientID']
        # print dic['accessionNO']

        vol = call_dicom_service('UIHPACSSERVER', '10.9.19.175', 3333, '205', 3333, '143606072595118', '01851591')
        call_ai_model(vol, 2, 0.8)
        ai_result_to_xml()

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
