#-*-coding:utf-8-*-
import os
from os import listdir
from pydicom import dcmread
import numpy as np
import md
import logging
from md.viewer.xviewer import *
import numpy.linalg as lina
import md.image3d.python.image3d_io as cio
import shutil
import ConfigParser

logger = logging.getLogger(__name__)
logger.setLevel(level=logging.INFO)
handler = logging.FileHandler("log.txt")
handler.setLevel(logging.INFO)
formatter = logging.Formatter('%(asctime)s - %(pathname)s - %(filename)s - '
                              '[line:%(lineno)d] - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
logger.addHandler(handler)

Config = ConfigParser.ConfigParser()
Config.read('config.ini')


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


class BuildVolume(object):
    """ build volume from DICOM files """

    def __init__(self):
        self.instance_pixels_position = []
        self.pixels = []
        self.ignore_slice_lost = True
        self.public_info = {}

    def reset(self):
        self.instance_pixels_position = []
        self.pixels = []
        self.ignore_slice_lost = True
        self.public_info = {}

    @log_enter_and_exit(arg=False)
    def __parse_validate(self, datasets):
        """check the DICOM datasets tags
        InstanceNumber, ImagePositionPatient and slice number
        """
        flag = True

        assert len(datasets) > 1, "datasets should have more than one slice,otherwise volume can't be built"

        # ensure each item in datasets_params which we will use in BuildVolume is not None
        dataset_0 = datasets[0]
        if 'BitsAllocated' not in dataset_0:
            flag = False
        if 'PixelRepresentation' not in dataset_0:
            flag = False
        if 'Modality' not in dataset_0:
            flag = False
        if 'RescaleSlope' not in dataset_0:
            flag = False
        if 'RescaleIntercept' not in dataset_0:
            flag = False

        dataset_0_params = [dataset_0.BitsAllocated, dataset_0.PixelRepresentation, dataset_0.Modality,
                            dataset_0.RescaleSlope, dataset_0.RescaleIntercept]
        for datasets_param in dataset_0_params:
            if datasets_param is None:
                flag = False

        self.public_info = {'StudyInstanceUID': dataset_0.StudyInstanceUID,
                            'AccessionNumber': dataset_0.AccessionNumber,
                            'PatientID': dataset_0.PatientID,
                            'SeriesInstanceUID': dataset_0.SeriesInstanceUID,
                            'SeriesNumber': dataset_0.SeriesNumber,
                            'PixelSpacing': np.array(dataset_0.PixelSpacing, np.float64),
                            'PatientName': dataset_0.PatientName,
                            'SeriesDescription': dataset_0.SeriesDescription}

        # ensure each item in dataset_params which we will use in BuildVolume is not None
        for dataset in datasets:
            if 'InstanceNumber' not in dataset:
                flag = False
            if 'PixelData' not in dataset:
                flag = False
            if 'ImagePositionPatient' not in dataset:
                flag = False

            dataset_params = [dataset.InstanceNumber, dataset.PixelData, dataset.ImagePositionPatient]
            for dataset_param in dataset_params:
                if dataset_param is None:
                    flag = False

        if flag is True:
            for dataset in datasets:
                instance_number = int(dataset.InstanceNumber)
                pixels_data = dataset.PixelData
                image_position = np.array(dataset.ImagePositionPatient, dtype=np.float64)
                image_uid = dataset.SOPInstanceUID
                tags_list = [instance_number, pixels_data, image_position, image_uid]
                self.instance_pixels_position.append(tags_list)
            self.instance_pixels_position = sorted(self.instance_pixels_position, key=lambda x: x[0])

            # ensure the InstanceNumber is continuous
            par_list = self.instance_pixels_position
            for i in range(len(par_list)-1):
                if par_list[i+1][0] - par_list[i][0] != 1:
                    raise ValueError("Dicom InstanceNumber is not continuous, volume will not be built")
                else:
                    logger.info("InstanceNumber is continuous, volume can be built!")

            # ensure the z direction slice spacing identical
            # attention: here ,we keep two digits after the decimal point
            for i in range(len(par_list)-2):
                if np.around(lina.norm(par_list[i+1][2]-par_list[i][2]), decimals=2) != \
                        np.around(lina.norm(par_list[i+2][2] - par_list[i+1][2]), decimals=2):
                    raise ValueError("z direction slice spacing are not equal, volume will not be built")
            self.pixels = [data[1] for data in self.instance_pixels_position]
            return flag

        else:
            flag = False
            return flag

    @log_enter_and_exit(arg=False)
    def __get_pixels_data_type(self, dataset):
        """
        get the pixel BitsAllocated and PixelRepresentation
        :param datasets: dicom files
        :return: pixiel datatype
        """
        bits_allocated = dataset.BitsAllocated
        pixel_representation = dataset.PixelRepresentation
        if bits_allocated == 8 and pixel_representation == 0:
            datatype = np.uint8
            return datatype
        elif bits_allocated == 8 and pixel_representation == 1:
            datatype = np.int8
            return datatype
        elif bits_allocated == 16 and pixel_representation == 0:
            datatype = np.uint16
            return datatype
        elif bits_allocated == 16 and pixel_representation == 1:
            datatype = np.int16
            return datatype
        elif bits_allocated == 32:
            datatype = np.complex64
            return datatype
        elif bits_allocated == 64:
            datatype = np.complex128
            return datatype

    @log_enter_and_exit(arg=False)
    def __check_modality(self, datasets):
        """
        check the dicom modality and get the RescaleSlope and RescaleIntercept
        :param datasets: dicom files
        :return: RescaleSlope,RescaleIntercept
        """
        dic = {}
        if datasets[0].Modality == 'CT':
            dic['RescaleSlope'] = float(datasets[0].RescaleSlope)
            dic['RescaleIntercept'] = float(datasets[0].RescaleIntercept)
        elif datasets[0].Modality == 'MR':
            dic['RescaleSlope'] = 1
            dic['RescaleIntercept'] = 0
        return dic

    @log_enter_and_exit(arg=False)
    def __pixels_to_image3d(self, images):
        """

        :param images:
        :return:
        """

        SeriesInstanceUID = images[0].SeriesInstanceUID
        mypath = Config.get("db", 'dicom_save_dir') + str(SeriesInstanceUID)
        dslist = []
        for f in listdir(mypath):
            file_path = os.path.join(mypath, f)
            with open(file_path, 'rb') as F:
                dataset = dcmread(F, force=True)
            dslist.append(dataset)
        datasets = dslist

        self.pixels = np.array(self.pixels)
        self.pixels = np.frombuffer(self.pixels, dtype=self.__get_pixels_data_type(datasets[0]), count=-1, offset=0)
        module = self.__check_modality(datasets)
        self.pixels = self.pixels * module['RescaleSlope'] + module['RescaleIntercept']
        self.pixels = np.reshape(self.pixels,
                                 (len(self.instance_pixels_position), datasets[0].Rows, datasets[0].Columns))
        im = md.Image3d()

        # set the origin of Image3D Volume
        im.from_numpy(self.pixels, np.short)
        origin = self.instance_pixels_position[0][2]
        im.set_origin(origin)

        # set the pixel spacing of Image3D Volume
        x_spacing = datasets[0].PixelSpacing[0]
        y_spacing = datasets[0].PixelSpacing[1]
        z_spacing = lina.norm(np.array(self.instance_pixels_position[-1][2])
                              - np.array(self.instance_pixels_position[0][2]))/(len(self.instance_pixels_position)-1)
        im.set_spacing([x_spacing, y_spacing, z_spacing])

        # set the axes of Image3D Volume
        orientation = datasets[0].ImageOrientationPatient
        axis_x = np.array([orientation[0], orientation[1], orientation[2]], np.float32)
        axis_y = np.array([orientation[3], orientation[4], orientation[5]], np.float32)
        axis_z = np.array(self.instance_pixels_position[1][2])-np.array(self.instance_pixels_position[0][2], np.float32)
        axis_z = axis_z/(lina.norm(axis_z))
        axes = [axis_x, axis_y, axis_z]
        axes = np.reshape(axes, (3, 3))
        im.set_axes(axes)
        print im
        print 'self_img'
        shutil.rmtree(mypath)
        return im

    @log_enter_and_exit(arg=False)
    def __pixels_to_image3d_md(self, datasets):
        """
        :param datasets:
        :return:
        """
        SeriesInstanceUID = datasets[0].SeriesInstanceUID
        dir = Config.get("db", 'dicom_save_dir') + str(SeriesInstanceUID)
        vol, tags = cio.read_dicom_series(dir)
        print vol
        shutil.rmtree(dir)

        return vol

    @log_enter_and_exit(arg=False)
    def volume(self, datasets):
        """Build Image3d Volume from one series dicom files
        :param datasets: one series dicom files
        :return: image3D Volume
        """
        validate = self.__parse_validate(datasets)
        if validate is True:
            if Config.getint("db", 'build_volume_from_md'):
                vol = self.__pixels_to_image3d_md(datasets)
            else:
                vol = self.__pixels_to_image3d(datasets)
        else:
            vol = None
        return vol


if __name__ == '__main__':
    # mypath = r'D:\dcm\1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373' #CT files
    mypath = r'D:\dcm\c_get\1.2.840.113704.1.111.12164.1508053864.7' #MR files
    dslist = []
    for f in listdir(mypath):
        file_path = os.path.join(mypath, f)
        with open(file_path, 'rb') as F:
            dataset = dcmread(F, force=True)
        dslist.append(dataset)

    a = BuildVolume()
    im = a.volume(dslist)

    # viewim(im)
