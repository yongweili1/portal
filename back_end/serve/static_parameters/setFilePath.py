# -*-coding:utf-8-*-
from __future__ import unicode_literals

import os

from django.core.files.storage import FileSystemStorage


class LinuxFilePath(FileSystemStorage):
    basePath = '/home/lyw/work/git_local_repository/portal'
    dicomPath = os.path.join(basePath, 'dicoms/')
    splitDicomPath = os.path.join(basePath, 'split_dicom/')
    volumePath = os.path.join(basePath, 'volume/')
    cpsPath = os.path.join(basePath, 'cps/')


class WindowsFilePath(FileSystemStorage):
    dicomPath = 'D:/dicoms/'
    splitDicomPath = 'D:/split_dicom/'
    volumePath = 'D:/volume/'
    cpsPath = 'D:/cps/'


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


if __name__ == '__main__':
    print LinuxFilePath().dicomPath