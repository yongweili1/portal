# -*- coding: utf-8 -*-
import os

from md.dicom.python.build_volume import BuildVolume
from md.image3d.python.image3d_io import write_image

from back_end.util.setFilePath import SaveVolumeFilePath
from back_end.util.readDcm import DcmPatient, DcmStudy, DcmSeries
from patientinformations.models import Series, Image


class DicomToVolume(object):

    def dicom_to_volume(self, datasetlist):
        """
        dicom to volume and upload volume to DB
        :param datasetlist:
        :return:volume file path
        """

        # 构造文件名
        dataset = datasetlist[0]
        a = DcmPatient()
        patient_dic = a.get_dicom_patient(dataset)
        b = DcmStudy()
        study_dic = b.get_dicom_study(dataset)
        c = DcmSeries()
        series_dic = c.get_dicom_series(dataset)

        volpath = patient_dic['patientname'] + '-' + study_dic['studydescription'] + '-' + \
                  series_dic['modality'] + '.nii.gz'

        # 调用dicom to volume方法，获取响应数据
        budvol = BuildVolume()
        vol = budvol.volume(1, datasetlist, 1)

        # 保存文件到本地
        volfilepath = os.path.join(SaveVolumeFilePath.volumepath, volpath)
        if not write_image(vol, volfilepath):
            pass

        return volfilepath