# -*-coding:utf-8-*-
import os

from serve.static_parameters.setFilePath import SaveDicomFilePath
from serve.util.readDcm import DcmSeries


class SplitDicoms(object):

    def split_series(self, file_name, dataset):
        """
        Distinguish DICOM files according to seriesuid
        :param patientpath:patient folder path
        :param file_name: dcm filename
        :param dataset:dicom filedataset
        :return:a series path of this upload
        """

        dcmseries = DcmSeries()
        seriesuid = dcmseries.get_dicom_series(dataset)['seriesuid']

        seriespath = SaveDicomFilePath.location_3 + str(seriesuid)
        if os.path.exists(seriespath):
            pass
        else:
            os.mkdir(seriespath)

        self.save_dicom(file_name, seriespath)

        return seriespath

    def save_dicom(self, file_name, seriespath):
        """
        Save dicom data
        :param file_name: dcm filename
        :param seriespath:series folder path
        :return:
        """
        # file_name = [k for k, v in filename_dataset_dic.items() if v == dataset][0]
        file_path = os.path.join(SaveDicomFilePath.location_2, file_name)
        with open(file_path, 'rb') as f:
            filedata = f.read()
        with open(seriespath + '\\' + file_name, 'wb+') as r:
            r.write(filedata)

