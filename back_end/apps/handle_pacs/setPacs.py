# -*-coding:utf-8-*-
class PacsConf(object):

    def __init__(self):
        self.pacs_ae_title = 'UIHPACSSERVER'
        self.pacs_ip = '10.9.19.150'
        self.pacs_port = 3333

        self.client_ae_title = 'LY'
        self.client_port = 3333

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


pacsconf = PacsConf()
