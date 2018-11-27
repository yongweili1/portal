# -*-coding:utf-8-*-
import os

from config.path_cfg import file_path_ferry
from utils.dcm_reader import DcmSeries


class DcmSorter(object):

    def split_series(self, file_name, dataset):
        dcmseries = DcmSeries()
        series_uid = dcmseries.get_series_uid(dataset)
        image_uid = dcmseries.get_image_uid(dataset)

        seriespath = os.path.join(file_path_ferry.splitDicomPath + str(series_uid))
        if not os.path.exists(seriespath):
            os.mkdir(seriespath)

        file_name = os.path.basename(file_name)

        self.save_dicom(file_name, os.path.join(seriespath, image_uid))

        return seriespath

    def save_dicom(self, file_name, new_file_name):
        """
        Save dicom data
        :param file_name: dcm filename
        :param seriespath:series folder path
        :return:
        """
        # file_name = [k for k, v in filename_dataset_dic.items() if v == dataset][0]
        file_path = os.path.join(file_path_ferry.dicomPath, file_name)
        with open(file_path, 'rb') as f:
            filedata = f.read()
        with open(new_file_name, 'wb+') as r:
            r.write(filedata)
