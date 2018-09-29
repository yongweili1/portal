# -*-coding:utf-8-*-
import os

from back_end.util.setFilePath import SaveDicomFilePath
from back_end.util.readDcm import DcmSeries, DcmPatient


class SplitDicoms(object):

    def __init__(self):
        self.patientidList = []
        self.seriesuidList = []

    def split_patient(self, filename_dataset_dic, dataset):
        """
        Make patient folder according to patientid
        :param filename_dataset_dic: mapping between filename and dataset
        :param dataset:dicom filedataset
        :return:a series path of this upload
        """

        dcmpatient = DcmPatient()
        dcmseries = DcmSeries()
        patientid = dcmpatient.get_dicom_patient(dataset)['patientid']
        seriesuid = dcmseries.get_dicom_series(dataset)['seriesuid']
        # if patientid not in self.patientidList:
        #     self.patientidList.append(patientid)
        # patientidSet = set(self.patientidList)

        # for patient_id in patientidSet:
        patientpath = SaveDicomFilePath.location_3 + str(patientid)
        if os.path.exists(patientpath):
            seriespath = self.split_series(patientpath, filename_dataset_dic, dataset)
        else:
            os.mkdir(patientpath)
            seriespath = self.split_series(patientpath, filename_dataset_dic, dataset)

        return seriespath

    def split_series(self, patientpath, filename_dataset_dic, dataset):
        """
        Distinguish DICOM files according to seriesuid
        :param patientpath:patient folder path
        :param filename_dataset_dic:mapping between filename and dataset
        :param dataset:dicom filedataset
        :return:a series path of this upload
        """

        dcmseries = DcmSeries()
        seriesuid = dcmseries.get_dicom_series(dataset)['seriesuid']

        # if seriesuid not in self.seriesuidList:
        #     self.seriesuidList.append(seriesuid)
        # seriesuidSet = set(self.seriesuidList)

        # dcmseries = DcmSeries()
        # for series_uid in seriesuidSet:
        seriespath = patientpath + '\\' + str(seriesuid)
        if os.path.exists(seriespath):
            # seruid = dcmseries.get_dicom_series(dataset)['seriesuid']
            # if seriesuid != seruid:
            #     continue
            self.save_dicom(filename_dataset_dic, seriespath, dataset)

        else:
            os.mkdir(seriespath)
            # seruid = dcmseries.get_dicom_series(dataset)['seriesuid']
            # if seriesuid != seruid:
            #     continue
            self.save_dicom(filename_dataset_dic, seriespath, dataset)

        return seriespath

    def save_dicom(self, filename_dataset_dic, seriespath, dataset):
        """
        Save dicom data
        :param filename_dataset_dic:mapping between filename and dataset
        :param seriespath:series folder path
        :param dataset:dicom filedataset
        :return:
        """
        file_name = [k for k, v in filename_dataset_dic.items() if v == dataset][0]
        file_path = os.path.join(SaveDicomFilePath.location_2, file_name)
        with open(file_path, 'rb') as f:
            filedata = f.read()
        with open(seriespath + '\\' + file_name, 'wb+') as file:
            file.write(filedata)

