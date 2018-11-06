# -*- coding: utf-8 -*-
import os

# from md.image3d.python.image3d_io import BuildVolume
from md.image3d.python.image3d_io import write_image, read_dicom_series

from serve.static_parameters.setFilePath import filepath


class DicomToVolume(object):

    def dicom_to_volume(self, seriespath):
        """
        dicom to volume and upload volume to DB
        :return:volume file path
        """
        vol, tags = read_dicom_series(seriespath)
        seriesuid = tags['0020|000e']
        volume_name = seriesuid + '.nii.gz'

        # 保存文件到本地
        if os.path.exists(filepath.volumePath):
            pass
        else:
            os.mkdir(filepath.volumePath)
        volfilepath = os.path.join(filepath.volumePath, volume_name)
        if not write_image(vol, volfilepath):
            pass

        return volfilepath, seriesuid