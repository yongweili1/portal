import os
import sys
import logging
from os import listdir
from pydicom import dcmread
from pydicom.uid import (
    ExplicitVRLittleEndian, ImplicitVRLittleEndian,
    ExplicitVRBigEndian, DeflatedExplicitVRLittleEndian
)

from pynetdicom3 import AE, StoragePresentationContexts

logger = logging.getLogger(__name__)
logger.setLevel(level=logging.INFO)
handler = logging.FileHandler("log.txt")
handler.setLevel(logging.INFO)
formatter = logging.Formatter('%(asctime)s - %(pathname)s - %(filename)s - '
                              '[line:%(lineno)d] - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
logger.addHandler(handler)


class CStoreScu(object):
    def __init__(self, pacs_ae_title, pacs_ip, pacs_port, client_ae_title, client_port):
        self.pacs_ae_title = pacs_ae_title
        self.pacs_ip = pacs_ip
        self.pacs_port = pacs_port
        self.client_ae_title = client_ae_title
        self.client_port = client_port
        self.transfer_syntax = [ExplicitVRLittleEndian,
                                ImplicitVRLittleEndian,
                                DeflatedExplicitVRLittleEndian,
                                ExplicitVRBigEndian]

    def __setup_c_store_assoc(self):
        ae = AE(ae_title=self.client_ae_title, port=self.client_port)
        for context in StoragePresentationContexts:
            ae.add_requested_context(context.abstract_syntax, self.transfer_syntax)
        assoc = ae.associate(self.pacs_ip, self.pacs_port, ae_title=self.pacs_ae_title)
        return assoc

    def store(self, folder_path):
        assoc = self.__setup_c_store_assoc()

        if assoc.is_established:
            try:
                for f in listdir(folder_path):
                    file_path = os.path.join(folder_path, f)
                    with open(file_path, 'rb') as F:
                        ds = dcmread(F, force=True)
                        assoc.send_c_store(ds)
            except IOError:
                logger.error("failed to read file:" + f)
                sys.exit()

            assoc.release()
        else:
            logger.error("failed to setup store association")



if __name__ == '__main__':
    a = CStoreScu(pacs_ae_title='ceshi', pacs_ip='192.168.232.78', pacs_port=3333,
                  client_ae_title='ceshi', client_port=3333)
    a.store(r'D:\1')

    # a = CStoreScu(pacs_ae_title='WQ2', pacs_ip='10.9.19.119', pacs_port=104,
    #               client_ae_title='WQ', client_port=3333)
    # a.store(r'D:\dcm\c_get\1.2.840.113704.1.111.12164.1508053864.7')

    print 'completed'

