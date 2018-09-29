# -*-coding:utf-8-*-
from __future__ import unicode_literals
from django.core.files.storage import FileSystemStorage


# 上传dicom文件的存储路径
class SaveDicomFilePath(FileSystemStorage):
    location_1 = 'D:\\temp\\'
    location_2 = 'D:\\dicoms\\'
    location_3 = 'D:\\split_dicom\\'


class SaveVolumeFilePath(FileSystemStorage):
    volumepath = 'D:\\volume\\'
