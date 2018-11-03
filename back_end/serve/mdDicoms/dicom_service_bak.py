from c_echo_scu import CEchoScu
from c_find_scu import CFindScu
from c_get_scu import CGetScu
from build_volume import BuildVolume
import logging
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


Config = ConfigParser.ConfigParser()
Config.read('config.ini')


class DicomService(object):
    def __init__(self, pacs_ae_title, pacs_ip, pacs_port, client_ae_title, client_port):
        self.pacs_ae_title = pacs_ae_title
        self.pacs_ip = pacs_ip
        self.pacs_port = pacs_port
        self.client_ae_title = client_ae_title
        self.client_port = client_port

        self.dcm_file_path = Config.get("db", 'dicom_save_dir')
        self.need_anonymize = 0
        self.need_save_file = 1
        self.retrieve_interval = 0
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
        self.c_echo.ping()

    #   c-find
    @log_enter_and_exit()
    def find_patients(self, patient_name, modality, date_range, patient_sex):
        patients = self.c_find.find_patients(patient_name=patient_name, modality=modality, date_range=date_range,
                                             patient_sex=patient_sex)
        return patients

    @log_enter_and_exit()
    def find_studies_by_patient_id(self, patient_id):
        studies = self.c_find.find_studies_by_patient_id(patient_id)
        return studies

    @log_enter_and_exit()
    def find_studies_by_patient_id_and_accession_number(self, patient_id, accession_number):
        studies = self.c_find.find_studies_by_patient_id_and_accession_number(patient_id, accession_number)
        return studies

    @log_enter_and_exit()
    def find_series_by_study_uid(self, study_uid):
        series = self.c_find.find_series_by_study_uid(study_uid)
        return series

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

    #   build volume
    # @log_enter_and_exit()
    def build_volume(self, ds_list):
        a = BuildVolume()
        # im = a.volume(1, ds_list, self.slice_lost_still_build)
        im = a.volume(ds_list)
        return im


if __name__ == '__main__':
    a = DicomService(pacs_ae_title='med_imFIR', pacs_ip='192.168.231.18', pacs_port=104,
                     client_ae_title='MIG_Pacs', client_port=10455
                     )
    a.connect()

    # b = a.find_patients(patient_name="", modality="", date_range="", patient_sex="")
    # for c in b:
    #     print(c)
    # b = a.find_studies_by_patient_id_and_accession_number('143606072595118', '01851591')
    b = a.find_studies_by_patient_id_and_accession_number('R1107693', '10306916')
    # print b
    c = a.find_series_by_study_uid(b)
    # print c
    d = a.get_series_by_uid(c)
    # print d
    # e = a.build_volume(d)
    # print e

    # d = a.find_images_by_series_uid(c)
    # k = ""
    # for dd in d:
    #     k = dd
    #     print k
    # a.get_image_by_uid(k)
    # a.get_image_by_patient_id('20160923-122320-991')
    # b = a.c_get.pixels
    # print b
    # a.move_image_by_uid(k)
    # a.move_image_by_patient_id('20160923-122320-991')
    # a.move_series_by_patient_id('20160923-122320-991')
