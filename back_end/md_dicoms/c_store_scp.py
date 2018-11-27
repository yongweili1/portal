import datetime
import os
import socket
import sys
import time

from pydicom.dataset import Dataset, FileDataset
from pydicom.uid import (
    ExplicitVRLittleEndian, ImplicitVRLittleEndian,
    ExplicitVRBigEndian, DeflatedExplicitVRLittleEndian
)
from pynetdicom3 import (
    AE,
    StoragePresentationContexts,
    VerificationPresentationContexts,
    QueryRetrievePresentationContexts,
    PYNETDICOM_IMPLEMENTATION_UID,
    PYNETDICOM_IMPLEMENTATION_VERSION
)


# from anonymize import Anonymize
# from parsedicom.anonymize import Anonymize


class CStoreScp(object):
    dcm_file_path = r'D:\tmp'
    # dcm_file_path = r'/tmp/dcms/'
    need_anonymize = 0
    need_save_file = 1
    pixels = []

    def __init__(self, store_ae_title, store_ae_port, store_ae_ip):
        self.store_ae_port = store_ae_port
        self.ae = AE(ae_title=store_ae_title, port=store_ae_port)
        self.ae.bind_addr = store_ae_ip
        pass

    def append_pixel(self, pixel):
        self.pixels.append(pixel)

    def on_association_accepted(self, primitive):
        print "client call on_association_accepted"
        print datetime.datetime.now()
        print primitive
        time.sleep(1)

    def on_association_released(self, primitive=None):
        print "client call on_association_released"
        print datetime.datetime.now()
        print primitive

    def on_association_aborted(self, primitive=None):
        print "client call on_association_aborted"
        print primitive

    def on_association_rejected(self, primitive):
        print "client call on_association_rejected"

    def on_n_set(self, context, info):
        print "client call on_n_set"
        print info

    def on_c_move_cancel(self):
        print "client call on_c_move_cancel"

    def on_association_requested(self, primitive):
        print "client call on_association_requested"
        print primitive

    def on_receive_connection(self):
        """Callback for a connection is received."""
        print "client call on_receive_connection"

    def on_make_connection(self):
        """Callback for a connection is made."""
        print "client call on_make_connection"

    def on_c_echo(self, context, info):
        print "client call on_c_echo"
        print info
        """Optional implementation of the AE.on_c_echo callback."""
        # Return a Success response to the peer
        # We could also return a pydicom Dataset with a (0000, 0900) Status
        #   element
        return 0x0000

    def on_c_find(self, dataset, context, info):
        print "client call on_c_find"

    def on_c_get(self, dataset, context, info):
        print "client call on_c_get"

    def on_c_move(self, dataset, move_aet, context, info):
        print "client call on_c_move"

    def on_c_store(self, dataset, context, info):
        print "client call on_c_store"
        #  do anonymize
        #   append pixel
        self.append_pixel(dataset.PixelData)

        if self.need_save_file:
            mode_prefixes = {'CT Image Storage': 'CT',
                             'Enhanced CT Image Storage': 'CTE',
                             'MR Image Storage': 'MR',
                             'Enhanced MR Image Storage': 'MRE',
                             'Positron Emission Tomography Image Storage': 'PT',
                             'Enhanced PET Image Storage': 'PTE',
                             'RT Image Storage': 'RI',
                             'RT Dose Storage': 'RD',
                             'RT Plan Storage': 'RP',
                             'RT Structure Set Storage': 'RS',
                             'Computed Radiography Image Storage': 'CR',
                             'Ultrasound Image Storage': 'US',
                             'Enhanced Ultrasound Image Storage': 'USE',
                             'X-Ray Angiographic Image Storage': 'XA',
                             'Enhanced XA Image Storage': 'XAE',
                             'Nuclear Medicine Image Storage': 'NM',
                             'Secondary Capture Image Storage': 'SC'}

            try:
                mode_prefix = mode_prefixes[dataset.SOPClassUID.name]
            except KeyError:
                mode_prefix = 'UN'

            filename = '{0!s}.{1!s}.dcm'.format(mode_prefix, dataset.SOPInstanceUID)

            meta = Dataset()
            meta.MediaStorageSOPClassUID = dataset.SOPClassUID
            meta.MediaStorageSOPInstanceUID = dataset.SOPInstanceUID
            meta.ImplementationClassUID = PYNETDICOM_IMPLEMENTATION_UID
            meta.TransferSyntaxUID = context.transfer_syntax

            # The following is not mandatory, set for convenience
            meta.ImplementationVersionName = PYNETDICOM_IMPLEMENTATION_VERSION

            ds = FileDataset(filename, {}, file_meta=meta, preamble=b"\0" * 128)
            ds.update(dataset)
            ds.is_little_endian = context.transfer_syntax.is_little_endian
            ds.is_implicit_VR = context.transfer_syntax.is_implicit_VR

            file_path = os.path.join(self.dcm_file_path, dataset.SeriesInstanceUID)
            if not os.path.exists(file_path):
                os.mkdir(file_path)
            filename = os.path.join(file_path, filename)
            ds.save_as(filename, write_like_original=False)

        status_ds = Dataset()
        status_ds.Status = 0x0000  # Success

        return status_ds

    def start(self):
        print 'c-store scp starting'
        transfer_syntax = [ImplicitVRLittleEndian,
                           ExplicitVRLittleEndian,
                           DeflatedExplicitVRLittleEndian,
                           ExplicitVRBigEndian]
        for context in StoragePresentationContexts:
            self.ae.add_supported_context(context.abstract_syntax, transfer_syntax)
        for context in VerificationPresentationContexts:
            self.ae.add_supported_context(context.abstract_syntax, transfer_syntax)
        for context in QueryRetrievePresentationContexts:
            self.ae.add_supported_context(context.abstract_syntax, transfer_syntax)

        self.ae.maximum_pdu_size = 528384
        # Set timeouts
        self.ae.network_timeout = None
        self.ae.acse_timeout = None
        self.ae.dimse_timeout = None

        self.ae.on_c_store = self.on_c_store

        self.ae.on_association_accepted = self.on_association_accepted
        self.ae.on_association_released = self.on_association_released
        self.ae.on_association_aborted = self.on_association_aborted
        self.ae.on_association_rejected = self.on_association_rejected
        self.ae.on_association_requested = self.on_association_requested
        self.ae.on_receive_connection = self.on_receive_connection
        self.ae.on_make_connection = self.on_make_connection
        self.ae.on_c_echo = self.on_c_echo
        self.ae.on_c_find = self.on_c_find
        self.ae.on_c_get = self.on_c_get
        self.ae.on_c_move = self.on_c_move

        self.ae.on_n_set = self.on_n_set
        self.ae.on_c_move_cancel = self.on_c_move_cancel

        print "c-store started!"
        try:
            self.ae.start()
        except:
            print "Uncexpected error", sys.exc_info()[0]

    def port_validation(self):
        if isinstance(self.store_ae_port, int):
            test_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            test_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            try:
                test_socket.bind((os.popen('hostname').read()[:-1], self.store_ae_port))
                print("Now listen on port {0:d}".format(self.store_ae_port))
            except socket.error:
                print("Cannot listen on port {0:d}, insufficient priveleges".format(self.store_ae_port))


if __name__ == '__main__':
    a = CStoreScp(store_ae_title='ceshi', store_ae_port=3333, store_ae_ip='192.168.232.78')
    # a = CStoreScp(store_ae_title='SGXQ2', store_ae_port=104, store_ae_ip='10.9.19.150')
    a.port_validation()
    a.start()
    # a.on_c_store()
