# -*- coding: utf-8 -*-
import os

# from md.image3d.python.image3d_io import BuildVolume
from md.image3d.python.image3d_io import write_image, read_dicom_series

from back_end.util.setFilePath import SaveVolumeFilePath


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
        volfilepath = os.path.join(SaveVolumeFilePath.volumePath, volume_name)
        if not write_image(vol, volfilepath):
            pass

        return volfilepath, seriesuid


        # try:
        #     vol, tags = read_dicom_series(seriespath)
        #     seriesuid = tags['0020|000e']
        #     volume_name = seriesuid + '.nii.gz'
        #     volfilepath = os.path.join(SaveVolumeFilePath.volumePath, volume_name)
        #     if not write_image(vol, volfilepath):
        #         pass
        #     print('build success')
        # except Exception as e:
        #     print('build error')


if __name__ == '__main__':
    # path = 'D:\\split_dicom\\143606072595118\\1.2.840.113704.1.111.12164.1508053864.7'
    path = 'D:\\dcm\\c_get\\1.2.156.112605.14038005400494.20180103021126.3.6404.5'
    aa = DicomToVolume()
    aa.dicom_to_volume(path)
