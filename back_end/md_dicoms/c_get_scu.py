import logging
import os
import time

from pydicom.dataset import Dataset, FileDataset
from pynetdicom3 import (
    AE,
    StoragePresentationContexts,
    QueryRetrievePresentationContexts,
    PYNETDICOM_IMPLEMENTATION_UID,
    PYNETDICOM_IMPLEMENTATION_VERSION
)

from c_find_scu import CFindScu

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


class CGetScu(object):
    def __init__(self, pacs_ae_title, pacs_ip, pacs_port, client_ae_title, client_port,
                 need_anonymize, need_save_file, dcm_file_path, retrieve_interval):
        self.pacs_ae_title = pacs_ae_title
        self.pacs_ip = pacs_ip
        self.pacs_port = pacs_port
        self.client_ae_title = client_ae_title
        self.client_port = client_port

        self.need_anonymize = need_anonymize
        self.need_save_file = need_save_file
        self.dcm_file_path = dcm_file_path
        self.retrieve_interval = retrieve_interval

        self.ds_list = []

        self.c_find = CFindScu(pacs_ae_title=pacs_ae_title, pacs_ip=pacs_ip, pacs_port=pacs_port,
                               client_ae_title=client_ae_title, client_port=client_port)

    @log_enter_and_exit()
    def set_need_anonymize(self, need_anonymize):
        self.need_anonymize = need_anonymize

    @log_enter_and_exit()
    def set_need_save_file(self, need_save_file):
        self.need_save_file = need_save_file

    @log_enter_and_exit()
    def set_dcm_file_path(self, dcm_file_path):
        self.dcm_file_path = dcm_file_path

    @log_enter_and_exit()
    def set_retrieve_interval(self, retrieve_interval):
        self.retrieve_interval = retrieve_interval

    @log_enter_and_exit()
    def append_pixel(self, pixel):
        self.pixels.append(pixel)

    @log_enter_and_exit(arg=False)
    def on_c_store(self, dataset, context, info):
        #  do anonymize

        #   append pixel
        self.ds_list.append(dataset)

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

            #   the remainder
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

    @log_enter_and_exit()
    def _setup_c_get_assoc(self):
        ae = AE(ae_title=self.client_ae_title, port=self.client_port)
        for context in QueryRetrievePresentationContexts:
            ae.add_requested_context(context.abstract_syntax)
        for context in StoragePresentationContexts:
            ae.add_requested_context(context.abstract_syntax)
        ae.on_c_store = self.on_c_store
        assoc = ae.associate(self.pacs_ip, self.pacs_port, ae_title=self.pacs_ae_title)
        return assoc

    # @log_enter_and_exit()
    def get_image_by_uid(self, image_uid):
        assoc = self._setup_c_get_assoc()

        if assoc.is_established:
            logger.info("get_image_by_uid association established")
            ds = Dataset()
            ds.SOPInstanceUID = image_uid
            ds.QueryRetrieveLevel = "IMAGE"
            response = assoc.send_c_get(ds, query_model='P')

            for status, identifier in response:
                pass
                # print status

            assoc.release()
        else:
            logger.error("get_image_by_uid association failed")

    @log_enter_and_exit()
    def get_images_by_series_uid(self, series_uid, retrieve_interval):
        self.ds_list = []
        images = self.c_find.find_images_by_series_uid(series_uid)
        logger.info("images:%s," % (len(images)))
        for image in images:
            self.get_image_by_uid(image)
            if retrieve_interval > 0:
                time.sleep(retrieve_interval)
        return self.ds_list

    @log_enter_and_exit()
    def get_series_by_uid(self, series_uid):
        self.ds_list = []
        assoc = self._setup_c_get_assoc()

        if assoc.is_established:
            logger.info("get_images_by_series_uid2 association established")
            ds = Dataset()
            ds.SeriesInstanceUID = series_uid
            ds.QueryRetrieveLevel = "SERIES"
            response = assoc.send_c_get(ds, query_model='P')

            for status, identifier in response:
                pass
                # print status
            # print 'slice count:', len(self.ds_list), datetime.utcnow()
            assoc.release()
            logger.info("get_images_by_series_uid2 association finished")
        else:
            logger.error("get_image_by_uid association failed")

        return self.ds_list
