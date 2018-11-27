import datetime
import time

from pydicom.dataset import Dataset
from pynetdicom3 import (
    AE,
    QueryRetrievePresentationContexts,
)


class CMoveScu(object):
    def __init__(self, pacs_ae_title, pacs_ip, pacs_port, client_ae_title, client_port, move_aet):
        self.pacs_ae_title = pacs_ae_title
        self.pacs_ip = pacs_ip
        self.pacs_port = pacs_port
        self.client_ae_title = client_ae_title
        self.client_port = client_port
        self.move_aet = move_aet

    def _setup_c_move_assoc(self):
        ae = AE(ae_title=self.client_ae_title, port=self.client_port)
        ae.requested_contexts = QueryRetrievePresentationContexts
        assoc = ae.associate(self.pacs_ip, self.pacs_port, ae_title=self.pacs_ae_title)
        return assoc

    def move_study_by_uid(self, study_uid):
        assoc = self._setup_c_move_assoc()

        if assoc.is_established:
            ds = Dataset()
            ds.StudyInstanceUID = study_uid
            ds.QueryRetrieveLevel = 'STUDY'
            response = assoc.send_c_move(ds, self.move_aet, query_model='S')

            for (status, identifier) in response:
                print status
                print datetime.datetime.now()
                print identifier
                # pass

            assoc.release()

    def move_series_by_uid(self, study_uid, series_uid):
        assoc = self._setup_c_move_assoc()

        if assoc.is_established:
            ds = Dataset()
            ds.StudyInstanceUID = study_uid
            ds.SeriesInstanceUID = series_uid
            ds.QueryRetrieveLevel = "SERIES"

            response = assoc.send_c_move(ds, self.move_aet, query_model='P')

            for (status, identifier) in response:
                print status
                print datetime.datetime.now()
                print identifier
                # pass

            assoc.release()

    def move_image_by_uid(self, study_uid, series_uid, image_uid):
        assoc = self._setup_c_move_assoc()

        if assoc.is_established:
            ds = Dataset()
            ds.SeriesInstanceUID = series_uid
            ds.StudyInstanceUID = study_uid
            ds.SOPInstanceUID = image_uid
            ds.QueryRetrieveLevel = "IMAGE"

            response = assoc.send_c_move(ds, self.move_aet, query_model='S')

            for (status, identifier) in response:
                print status
                print datetime.datetime.now()
                print identifier
                # if status.get(0x00000900).value == 65280:

            assoc.release()

    def move_series_by_image_uids(self, study_uid, series_uid, images, retrieve_interval):
        for image in images:
            print image
            self.move_image_by_uid(study_uid, series_uid, image)
            time.sleep(retrieve_interval)


if __name__ == '__main__':
    # a = DicomService(pacs_ae_title='UIHPACSSERVER', pacs_ip='10.9.19.150', pacs_port=3333,
    #                  client_ae_title='WQ', client_port=3333
    #                  )
    # a.connect()
    #
    # b = a.find_patients(patient_name="", modality="", date_range="", patient_sex="")
    # print 'patient list:'
    # for c in b:
    #     print(c)
    # print c.id
    # c1 = a.find_studies_by_patient_id(c.id)
    # print 'study list:'
    # for c in c1:
    #     print c
    # d1 = a.find_series_by_study_uid(c)
    # print 'series list:'
    # for d in d1[1]:
    #     print d
    # d = '1.2.156.112605.14038005400494.20180103071418.3.6404.5'
    # e1 = a.find_images_by_series_uid(d)
    # print 'image list:'
    # for e in e1:
    #     print e

    # kk = CMoveScu('UIHPACSSERVER', '10.9.19.150', 3333, 'SGXQ2', 104, '10.9.19.150')
    kk = CMoveScu('UIHPACSSERVER', '10.9.19.150', 3333, 'WQ2', 104, 'WQ')
    kk.move_series_by_uid('1.2.840.113704.1.111.12164.1508053864.7')

    print 'ok'
