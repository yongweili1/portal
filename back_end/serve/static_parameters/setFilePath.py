# -*-coding:utf-8-*-
from __future__ import unicode_literals
from django.core.files.storage import FileSystemStorage


class LinuxFilePath(FileSystemStorage):
    basePath = '../'
    dicomPath = basePath + 'dicoms'
    splitDicomPath = basePath + 'split_dicom'
    volumePath = basePath + 'volume'
    cpsPath = basePath + 'cps'


class WindowsFilePath(FileSystemStorage):
    dicomPath = 'D:\\dicoms\\'
    splitDicomPath = 'D:\\split_dicom\\'
    volumePath = 'D:\\volume\\'
    cpsPath = 'D:\\cps\\'


class SaveDicomFilePath(object):

    def __init__(self):
        self.dicomPath = ''
        self.splitDicomPath = ''
        self.volumePath = ''
        self.cpsPath = ''

    def set_dcmpath(self, dcmfilepath):
        self.dicomPath = dcmfilepath

    def set_splitdcmpath(self, splitfilepath):
        self.splitDicomPath = splitfilepath

    def set_volumepath(self, volumepath):
        self.volumePath = volumepath

    def set_cpspath(self, cpspath):
        self.cpsPath = cpspath


filepath = SaveDicomFilePath()
