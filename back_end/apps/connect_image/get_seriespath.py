# -*-coding:utf-8-*-
import os

from models import Series, Study, Patient
from back_end.util.setFilePath import SaveDicomFilePath


class GetSeriesPath(object):
    def getseriespath(self, seriesuid):
        seriespath = SaveDicomFilePath.location_3 + str(seriesuid)
        return seriespath


getpath = GetSeriesPath()
