# -*-coding:utf-8-*-
from __future__ import unicode_literals
from django.core.files.storage import FileSystemStorage


class LinuxFilePath(FileSystemStorage):
    basePath = '../../'
    dicomPath = basePath + 'dicoms'
    splitDicomPath = basePath + 'split_dicom'
    volumePath = basePath + 'volume'


class SaveDicomFilePath(FileSystemStorage):
    location_1 = 'D:\\temp\\'
    location_2 = 'D:\\dicoms\\'
    location_3 = 'D:\\split_dicom\\'


class SaveVolumeFilePath(FileSystemStorage):
    volumePath = 'D:\\volume\\'


class DotPositionFilePath(FileSystemStorage):
    dotPosPath = 'D:\\dotpostion\\'
