from pydicom.uid import (ExplicitVRLittleEndian, ImplicitVRLittleEndian, ExplicitVRBigEndian)

from pynetdicom3 import Association
from pynetdicom3.sop_class import VerificationSOPClass
from pynetdicom3 import AE
import logging

"""
conntet client AE to PACS
"""

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


class CEchoScu(object):
    def __init__(self, pacs_ae_title, pacs_ip, pacs_port, client_ae_title, client_port):
        self.pacs_ae_title = pacs_ae_title
        self.pacs_ip = pacs_ip
        self.pacs_port = pacs_port
        self.client_ae_title = client_ae_title
        self.client_port = client_port

    @log_enter_and_exit()
    def _setup_c_echo_assoc(self):
        transfer_syntax = [ImplicitVRLittleEndian,
                           ExplicitVRLittleEndian,
                           ExplicitVRBigEndian]
        ae = AE(ae_title=self.client_ae_title, port=self.client_port)
        ae.add_requested_context(VerificationSOPClass, transfer_syntax)
        assoc = ae.associate(self.pacs_ip, self.pacs_port, ae_title=self.pacs_ae_title)
        return assoc

    @log_enter_and_exit()
    def ping(self):
        assoc = self._setup_c_echo_assoc()
        assert isinstance(assoc, Association)
        if assoc.is_established:
            logger.info("c_echo association established")
            for ii in range(1):
                status = assoc.send_c_echo()
                print status
                return status.get(0x00000900).value
        else:
            logger.error("c_echo association failed")
            return 1

if __name__ == '__main__':
    a = CEchoScu(pacs_ae_title='med_imFIR', pacs_ip='192.168.231.18', pacs_port=104,
                client_ae_title='ceshi', client_port=3333)
    a.ping()
