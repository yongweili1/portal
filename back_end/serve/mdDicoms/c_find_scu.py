from pydicom.dataset import Dataset
from pynetdicom3 import AE, QueryRetrievePresentationContexts
import logging
import datetime
import string

from patient import Patient

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


class CFindScu(object):
    def __init__(self, pacs_ae_title, pacs_ip, pacs_port, client_ae_title, client_port):
        self.pacs_ae_title = pacs_ae_title
        self.pacs_ip = pacs_ip
        self.pacs_port = pacs_port
        self.client_ae_title = client_ae_title
        self.client_port = client_port

    @log_enter_and_exit()
    def _setup_c_find_assoc(self):
        ae = AE(ae_title=self.client_ae_title, port=self.client_port)
        ae.requested_contexts = QueryRetrievePresentationContexts
        assoc = ae.associate(self.pacs_ip, self.pacs_port, ae_title=self.pacs_ae_title)
        return assoc

    @log_enter_and_exit()
    def find_patients(self, patient_id, patient_name, patient_age, patient_sex, modality, date_range):
        patients = []
        assoc = self._setup_c_find_assoc()
        if assoc.is_established:
            logger.info("find_patients association established")
            ds = Dataset()
            if len(patient_id):
                ds.PatientID = patient_id
            if len(patient_name):
                ds.PatientName = "*" + patient_name + "*"
            if len(patient_age):
                ds.PatientAge = patient_age
            if len(patient_sex):
                ds.PatientSex = patient_sex
            if len(modality):
                ds.ModalitiesInStudy = modality
            if len(date_range):
                ds.StudyDate = date_range
            # if len(study_description):
            #     ds.StudyDescription = "*CHEST*"
            # if len(patient_birthdate_range):
            #     ds.PatientBirthDate = patient_birthdate_range
            ds.QueryRetrieveLevel = "STUDY"
            response = assoc.send_c_find(ds, query_model='S')
            for (status, identifier) in response:  # type: (object, object)
                if status.get(0x00000900).value == 65280:
                    if 'PatientName' in identifier:
                        p_name = identifier.get_item(0x00100010).value
                    else:
                        p_name = None
                    if 'PatientID' in identifier:
                        p_id = identifier.get_item(0x00100020).value
                    else:
                        p_id = None
                    if 'PatientAge' in identifier:
                        p_age = identifier.get_item(0x00101010).value
                    # elif 'PatientBirthDate' in identifier:               # birthdate to age
                    #     p_birthdate = identifier.get_item(0x00100030).value
                    #     p_birthyear = p_birthdate[:4]
                    #     year_now = datetime.datetime.now().year
                    #     p_age = str(int(year_now)-int(p_birthyear))
                    #     print p_birthdate + "  " + p_age
                    else:
                        p_age = None
                    if 'PatientSex' in identifier:
                        p_sex = identifier.get_item(0x00100040).value
                    else:
                        p_sex = None
                    if 'ModalitiesInStudy' in identifier:
                        p_modality = identifier.get_item(0x00080061).value
                        if p_modality == ['CT', 'FILM_CT']:
                            p_modality = "CT & FILM_CT"
                    else:
                        p_modality = None

                    patient = Patient(name=p_name, id=p_id, age=p_age, sex=p_sex, modality=p_modality)
                    patients.append(patient)

            assoc.release()
            if len(patients) == 0:
                logger.info("can't find patient with search conditions:", exc_info=True)
        else:
            logger.error("find_patients association failed")
        return patients

    @log_enter_and_exit()
    def find_studies_by_patient_id(self, patient_id):
        studies = []
        assoc = self._setup_c_find_assoc()
        if assoc.is_established:
            ds = Dataset()
            ds.PatientID = patient_id
            ds.QueryRetrieveLevel = "STUDY"
            response = assoc.send_c_find(ds, query_model='S')
            for (status, identifier) in response:  # type: (object, object)
                if status.get(0x00000900).value == 65280:
                    studies.append(identifier.get_item(0x0020000D).value)

            assoc.release()
            if len(studies) == 0:
                logger.info("can't find studies with search conditions")
        else:
            logger.error("find_studies_by_patient_id association failed")

        return studies

    @log_enter_and_exit()
    def find_study_by_patient_id_and_accession_number(self, patient_id, accession_number):
        studies = []
        assoc = self._setup_c_find_assoc()
        if assoc.is_established:
            ds = Dataset()
            ds.PatientID = patient_id
            ds.StudyInstanceUID = ''
            ds.StudyDate = ''
            ds.StudyTime = ''
            if len(accession_number):
                ds.AccessionNumber = accession_number
            ds.QueryRetrieveLevel = "STUDY"
            response = assoc.send_c_find(ds, query_model='S')
            for (status, identifier) in response:
                if status.get(0x00000900).value == 65280:
                    studies.append(identifier)
                    # studies.append(identifier.get_item(0x0020000D).value)

            assoc.release()
            if len(studies) == 0:
                logger.info("can't find studies with search conditions")
            else:
                latest_study_date = studies[0].get_item(0x00080020).value
                latest_study_uid = studies[0].get_item(0x0020000D).value
                for study in studies:
                    if study.get_item(0x00080020).value > latest_study_date:
                        latest_study_uid = study.get_item(0x0020000D).value
        else:
            logger.error("find_studies_by_patient_id association failed")
        return latest_study_uid

    @log_enter_and_exit()
    def find_series_by_study_uid(self, study_uid):
        series = []
        assoc = self._setup_c_find_assoc()
        series_nums_dic = {}
        if assoc.is_established:
            logger.info("find_series_by_study_uid association established")
            ds = Dataset()
            ds.SeriesInstanceUID = ''
            ds.NumberOfSeriesRelatedInstances = ''
            ds.StudyInstanceUID = study_uid
            ds.QueryRetrieveLevel = "SERIES"
            response = assoc.send_c_find(ds, query_model='S')
            for (status, identifier) in response:  # type: (object, object)
                if status.get(0x00000900).value == 65280:
                    series.append(identifier.get_item(0x0020000E).value)
                    series_uid = identifier.get_item(0x0020000E).value
                    nums = identifier.get_item(0x00201209).value
                    series_nums_dic.update({series_uid: nums})

            assoc.release()
            if len(series) == 0:
                logger.info("can't find series with search conditions")
        else:
            logger.error("find_series_by_study_uid association failed")
        return series, series_nums_dic

    @log_enter_and_exit()
    def find_images_by_series_uid(self, series_uid):
        images = []
        assoc = self._setup_c_find_assoc()
        if assoc.is_established:
            logger.info("find_images_by_series_uid association established")
            ds = Dataset()
            ds.SOPInstanceUID = ''
            ds.SeriesInstanceUID = series_uid
            ds.QueryRetrieveLevel = "IMAGE"
            response = assoc.send_c_find(ds, query_model='S')
            for (status, identifier) in response:  # type: (object, object)
                if status.get(0x00000900).value == 65280:
                    images.append(identifier.get_item(0x00080018).value)

            if len(images) == 0:
                logger.info("can't find images with search conditions")
        else:
            logger.error("find_images_by_series_uid association failed")

            assoc.release()
        return images

    @log_enter_and_exit()
    def find_image(self, image_uid):
        assoc = self._setup_c_find_assoc()
        if assoc.is_established:
            logger.info("find_image association established")
            ds = Dataset()
            ds.SOPInstanceUID = image_uid
            ds.QueryRetrieveLevel = "IMAGE"
            response = assoc.send_c_find(ds, query_model='S')
            for (status, identifier) in response:  # type: (object, object)
                if status.get(0x00000900).value == 65280:
                    return identifier

            assoc.release()
        else:
            logger.error("find_image association failed")


if __name__ == '__main__':
    a = CFindScu(pacs_ae_title='med_imFIR', pacs_ip='192.168.231.18', pacs_port=104,
                 client_ae_title='ceshi', client_port=3333)
    b = a.find_study_by_patient_id_and_accession_number('', '')
    print b

    # b = a.find_series_by_study_uid('1.2.840.113564.19216823112.534.20181023115440500.121')
    # print 'series uid', b[0]
    # c = a.find_images_by_series_uid(b[0])
    # print 'image uid', c[0]

