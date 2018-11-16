# -*-coding:utf-8-*-
from serve.static_parameters.setFilePath import file_path_ferry

class PacsConf(object):

    def __init__(self):
        self.pacs_ae_title = 'UIHPACSSERVER'
        self.pacs_ip = '10.9.19.96'
        self.pacs_port = 3333

        self.client_ae_title = 'LY'
        self.client_port = 3333
        self.dcm_file_path = file_path_ferry.splitDicomPath

    def set_pacs_ae_title(self, pacs_ae_title):
        self.pacs_ae_title = pacs_ae_title

    def set_pacs_ip(self, pacs_ip):
        self.pacs_ip = pacs_ip

    def set_pacs_port(self, pacs_port):
        self.pacs_port = pacs_port

    def set_client_ae_title(self, client_ae_title):
        self.client_ae_title = client_ae_title

    def set_client_port(self, client_port):
        self.client_port = client_port

    def set_dcm_file_path(self, dcm_file_path):
        self.dcm_file_path = dcm_file_path


pacsconf = PacsConf()
