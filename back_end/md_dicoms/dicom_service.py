import ConfigParser
import logging
from threading import *

from build_volume import BuildVolume
from c_echo_scu import CEchoScu
from c_find_scu import CFindScu
from c_get_scu import CGetScu
from c_store_scp import CStoreScp
from c_store_scu import CStoreScu
from cmovescu_dcmtk import CMoveScu

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


Config = ConfigParser.ConfigParser()
Config.read('config.ini')


class DicomService(object):
    def __init__(self, pacs_ae_title, pacs_ip, pacs_port, client_ae_title, client_port, dcm_file_path):
        self.pacs_ae_title = pacs_ae_title
        self.pacs_ip = pacs_ip
        self.pacs_port = pacs_port
        self.client_ae_title = client_ae_title
        self.client_port = client_port

        self.dcm_file_path = dcm_file_path
        self.need_anonymize = 0
        self.need_save_file = 1
        self.retrieve_interval = 5
        self.pixels = []
        self.slice_lost_still_build = 1

        self.c_echo = CEchoScu(pacs_ae_title=pacs_ae_title, pacs_ip=pacs_ip, pacs_port=pacs_port,
                               client_ae_title=client_ae_title, client_port=client_port)
        self.c_find = CFindScu(pacs_ae_title=pacs_ae_title, pacs_ip=pacs_ip, pacs_port=pacs_port,
                               client_ae_title=client_ae_title, client_port=client_port)
        self.c_get = CGetScu(pacs_ae_title=pacs_ae_title, pacs_ip=pacs_ip, pacs_port=pacs_port,
                             client_ae_title=client_ae_title, client_port=client_port,
                             need_anonymize=self.need_anonymize, need_save_file=self.need_save_file,
                             dcm_file_path=self.dcm_file_path, retrieve_interval=self.retrieve_interval)
        self.c_move = CMoveScu(pacs_ae_title=pacs_ae_title, pacs_ip=pacs_ip, pacs_port=pacs_port,
                               client_ae_title=client_ae_title, client_port=client_port,
                               save_dir=self.dcm_file_path)
        self.c_store_scu = CStoreScu(pacs_ae_title=pacs_ae_title, pacs_ip=pacs_ip, pacs_port=pacs_port,
                                     client_ae_title=client_ae_title, client_port=client_port)

    @log_enter_and_exit()
    def set_need_anonymize(self, need_anonymize):
        self.need_anonymize = need_anonymize
        self.c_get.set_need_anonymize(need_anonymize)

    @log_enter_and_exit()
    def set_need_save_file(self, need_save_file):
        self.need_save_file = need_save_file
        self.c_get.set_need_save_file(need_save_file)

    @log_enter_and_exit()
    def set_dcm_file_path(self, dcm_file_path):
        self.dcm_file_path = dcm_file_path
        self.c_get.set_dcm_file_path(dcm_file_path)

    @log_enter_and_exit()
    def set_retrieve_interval(self, retrieve_interval):
        self.retrieve_interval = retrieve_interval
        self.c_get.set_retrieve_interval(retrieve_interval)

    #   c-echo
    @log_enter_and_exit()
    def connect(self):
        return self.c_echo.ping()

    #   c-find
    @log_enter_and_exit()
    def find_patients(self, patient_id, patient_name, patient_age, patient_sex, modality, date_range):
        patients = self.c_find.find_patients(patient_id=patient_id, patient_name=patient_name, patient_age=patient_age,
                                             patient_sex=patient_sex, modality=modality, date_range=date_range)
        return patients

    @log_enter_and_exit()
    def find_studies_by_patient_id(self, patient_id):
        studies = self.c_find.find_studies_by_patient_id(patient_id)
        return studies

    @log_enter_and_exit()
    def find_study_by_patient_id_and_accession_number(self, patient_id, accession_number):
        studies = self.c_find.find_study_by_patient_id_and_accession_number(patient_id, accession_number)
        return studies

    @log_enter_and_exit()
    def find_series_by_study_uid(self, study_uid):
        return self.c_find.find_series_by_study_uid(study_uid)

    @log_enter_and_exit()
    def find_images_by_series_uid(self, series_uid):
        images = self.c_find.find_images_by_series_uid(series_uid)
        return images

    #   c-get
    @log_enter_and_exit()
    def get_image_by_uid(self, image_uid):
        self.c_get.get_image_by_uid(image_uid)

    @log_enter_and_exit()
    def get_images_by_series_uid(self, series_uid):
        return self.c_get.get_images_by_series_uid(series_uid, self.retrieve_interval)

    def get_series_by_uid(self, series_uid):
        return self.c_get.get_series_by_uid(series_uid)

    #   c-store
    #   c-move
    def move_by_study_uid(self, study_uid):
        self.c_move.movescu_by_studyuid(study_uid)

    def move_by_series_uid(self, study_uid, series_uid):
        return self.c_move.movescu_by_seriesuid(study_uid, series_uid)

    def move_by_image_uid(self, study_uid, series_uid, image_uids):
        return self.c_move.movescu_by_imageuid(study_uid, series_uid, image_uids, self.retrieve_interval)

    #   build volume
    # @log_enter_and_exit()
    def build_volume(self, ds_list):
        a = BuildVolume()
        # im = a.volume(1, ds_list, self.slice_lost_still_build)
        im = a.volume(ds_list)
        return im


class WorkingThread2(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        # a = CStoreScp(store_ae_title='uii1', store_ae_port=7000, store_ae_ip='172.17.0.15')
        a = CStoreScp(store_ae_title='uii1', store_ae_port=7000, store_ae_ip='192.168.232.78')
        a.port_validation()
        a.start()


# thread2 = WorkingThread2()
# thread2.start()


if __name__ == '__main__':
    a = DicomService(pacs_ae_title='med_imFIR', pacs_ip='192.168.231.18', pacs_port=104,
                     client_ae_title='ceshi', client_port=3333)
    a.connect()

    # thread2 = WorkingThread2()
    # thread2.start()
    # a = DicomService(pacs_ae_title='med_imFIR', pacs_ip='192.168.231.18', pacs_port=104,
    #                  client_ae_title='ceshi', client_port=3333)
    # a.connect()

    b = a.find_studies_by_patient_id_and_accession_number('P10887939', 'A002437774')
    print 'study uid:', b

    c, num_dict = a.find_series_by_study_uid(b)
    print 'series uid:', c

    c = '1.3.12.2.1107.5.1.4.73787.30000018102300334331000076307'
    d = a.find_images_by_series_uid(c)
    print 'image uids:', d

    # c, seri_nums = a.find_series_by_study_uid(b)
    # print 'series uid:', c[0]

    a.move_by_image_uid(b[0], c[0], d)
    # a.move_by_series_uid(b[0], '1.3.12.2.1107.5.1.4.73787.30000018102300334331000076307')
