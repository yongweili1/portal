# -*-coding:utf-8-*-
import os

from back_end.util.setFilePath import SaveDicomFilePath
from back_end.util.readDcm import DcmSeries, DcmPatient


class SplitDicoms(object):

    def split_patient(self, file_name, dataset):
        """
        Make patient folder according to patientid
        :param file_name: dcm filename
        :param dataset:dicom filedataset
        :return:a series path of this upload
        """

        dcmpatient = DcmPatient()
        patientid = dcmpatient.get_dicom_patient(dataset)['patientid']

        patientpath = SaveDicomFilePath.location_3 + str(patientid)
        if os.path.exists(patientpath):
            pass
        else:
            os.mkdir(patientpath)
        seriespath = self.split_series(patientpath, file_name, dataset)

        return seriespath

    def split_series(self, patientpath, file_name, dataset):
        """
        Distinguish DICOM files according to seriesuid
        :param patientpath:patient folder path
        :param filename_dataset_dic:mapping between filename and dataset
        :param dataset:dicom filedataset
        :return:a series path of this upload
        """

        dcmseries = DcmSeries()
        seriesuid = dcmseries.get_dicom_series(dataset)['seriesuid']

        seriespath = patientpath + '\\' + str(seriesuid)
        if os.path.exists(seriespath):
            pass
        else:
            os.mkdir(seriespath)
        self.save_dicom(file_name, seriespath, dataset)

        return seriespath

    def save_dicom(self, file_name, seriespath, dataset):
        """
        Save dicom data
        :param filename_dataset_dic:mapping between filename and dataset
        :param seriespath:series folder path
        :param dataset:dicom filedataset
        :return:
        """
        # file_name = [k for k, v in filename_dataset_dic.items() if v == dataset][0]
        file_path = os.path.join(SaveDicomFilePath.location_2, file_name)
        with open(file_path, 'rb') as f:
            filedata = f.read()
        with open(seriespath + '\\' + file_name, 'wb+') as file:
            file.write(filedata)

