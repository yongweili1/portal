# -*-coding:utf-8-*-
from __future__ import unicode_literals

import os


class SaveDicomFilePath(object):
    def __init__(self):
        root = os.path.dirname(os.path.abspath(__file__)).split('portal')[0]
        root = os.path.join(root, 'portal_ferry')
        self.dicomPath = os.path.join(root, 'dicoms/')
        if not os.path.isdir(self.dicomPath):
            os.makedirs(self.dicomPath)

        self.splitDicomPath = os.path.join(root, 'split_dicom/')
        if not os.path.isdir(self.splitDicomPath):
            os.makedirs(self.splitDicomPath)

        self.volumePath = os.path.join(root, 'volume/')
        if not os.path.isdir(self.volumePath):
            os.makedirs(self.volumePath)

        self.cpsPath = os.path.join(root, 'cps/')
        if not os.path.isdir(self.cpsPath):
            os.makedirs(self.cpsPath)


file_path_ferry = SaveDicomFilePath()

if __name__ == '__main__':
    print file_path_ferry.dicomPath
    print file_path_ferry.splitDicomPath
    print file_path_ferry.volumePath
    print file_path_ferry.cpsPath
