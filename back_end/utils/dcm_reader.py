# -*-coding:utf-8-*-
import time
import pydicom
import datetime
import re
from enum import Enum
import logging


class DateTimeType(Enum):
    Date = 1
    Time = 2
    DateTime = 3


def get_date_time(type, inputdatetime):
    '''
    根据时间类型将输入数据转成数据库能够识别的格式
    :param type:
    :param inputdatetime:
    :return:
    '''
    if (type == DateTimeType.Date.value
        or type == DateTimeType.Time.value
        or type == DateTimeType.DateTime.value) \
            and len(inputdatetime) != 0:

        if type == DateTimeType.Date.value:
            pattern = re.compile(r'\d{4}(0\d|1[0-2])([0-2]\d|3[0-1])')
            match = pattern.match(inputdatetime)
            if match:
                outputdatetime = match.group()
                outputdatetime = time.strptime(outputdatetime, '%Y%m%d')
                outputdatetime = datetime.date(outputdatetime.tm_year, outputdatetime.tm_mon,
                                               outputdatetime.tm_mday)
            else:
                outputdatetime = None
                logging.error('the format of %s is wrong' % inputdatetime)

        elif type == DateTimeType.Time.value:
            pattern = re.compile(r'([0-1]\d|2[0-3])([0-5]\d)([0-5]\d)')
            match = pattern.match(inputdatetime)
            if match:
                outputdatetime = match.group()
                outputdatetime = time.strptime(outputdatetime, '%H%M%S')
                outputdatetime = datetime.time(outputdatetime.tm_hour, outputdatetime.tm_min, outputdatetime.tm_sec)
            else:
                outputdatetime = None
                logging.error('the format of %s is wrong' % inputdatetime)

        else:
            pattern = re.compile(r'\d{4}(0\d|1[0-2])([0-2]\d|3[0-1])([0-1]\d|2[0-3])([0-5]\d)([0-5]\d)')
            match = pattern.match(inputdatetime)
            if match:
                outputdatetime = match.group()
                outputdatetime = time.strptime(outputdatetime, '%Y%m%d%H%M%S')
                outputdatetime = datetime.datetime(outputdatetime.tm_year, outputdatetime.tm_mon,
                                                   outputdatetime.tm_mday, outputdatetime.tm_hour,
                                                   outputdatetime.tm_min, outputdatetime.tm_sec)
            else:
                outputdatetime = None
                logging.error('the format of %s is wrong' % inputdatetime)

    elif (type == DateTimeType.Date.value
          or type == DateTimeType.Time.value
          or type == DateTimeType.DateTime.value) \
            and len(inputdatetime) == 0:
        outputdatetime = None

    else:
        logging.error('input type must be enum')

    return outputdatetime


class DcmPatient(object):
    def __init__(self):
        self.dic = {'patientid': '',
                    'patientname': '',
                    'patientbirthdate': '',
                    'patientsex': '',
                    'patientsize': '',
                    'patientweight': '',
                    'patientcomments': '',
                    'importdatetime': '',
                    }

    # don't have pid, patientuid, updatetime
    def get_dicom_patient(self, dataset):
        if 'PatientID' in dataset:
            patientid = dataset.data_element("PatientID").value
        else:
            patientid = ''

        if 'PatientName' in dataset:
            patientname = dataset.data_element("PatientName").value
        else:
            patientname = ''

        if 'PatientBirthDate' in dataset:
            patientbirthdate = dataset.data_element("PatientBirthDate").value
            patientbirthdate = get_date_time(1, patientbirthdate)
        else:
            patientbirthdate = ''

        if 'PatientSex' in dataset:
            patientsex = dataset.data_element("PatientSex").value
        else:
            patientsex = ''

        if 'PatientSize' in dataset:
            patientsize = dataset.data_element("PatientSize").value
        else:
            patientsize = ''

        if 'PatientWeight' in dataset:
            patientweight = dataset.data_element("PatientWeight").value
        else:
            patientweight = ''

        if 'PatientComments' in dataset:
            patientcomments = dataset.data_element("PatientComments").value
        else:
            patientcomments = ''

        importdatetime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

        self.dic['patientid'] = patientid
        self.dic['patientname'] = patientname
        self.dic['patientbirthdate'] = patientbirthdate
        self.dic['patientsex'] = patientsex
        self.dic['patientsize'] = patientsize
        self.dic['patientweight'] = patientweight
        self.dic['patientcomments'] = patientcomments
        self.dic['importdatetime'] = importdatetime

        return self.dic


class DcmStudy(object):
    def __init__(self):
        self.dic = {'studyuid': '',
                    'patientid': '',
                    'studyid': '',
                    'studydate': '',
                    'studytime': '',
                    'studydescription': '',
                    }

    # don't have pid, patientuid, updatetime
    def get_dicom_study(self, dataset):
        if 'StudyInstanceUID' in dataset:
            studyuid = dataset.data_element("StudyInstanceUID").value
        else:
            studyuid = ''

        if 'StudyID' in dataset:
            studyid = dataset.data_element("StudyID").value
        else:
            studyid = ''

        if 'PatientID' in dataset:
            patientid = dataset.data_element("PatientID").value
        else:
            patientid = ''

        if 'StudyDate' in dataset:
            studydate = dataset.data_element("StudyDate").value
            studydate = get_date_time(1, studydate)
        else:
            studydate = None

        if 'StudyTime' in dataset:
            studytime = dataset.data_element("StudyTime").value
            studytime = get_date_time(2, studytime)
        else:
            studytime = None

        if 'StudyDescription' in dataset:
            studydescription = dataset.data_element("StudyDescription").value
        else:
            studydescription = None

        self.dic['studyuid'] = studyuid
        self.dic['studyid'] = studyid
        self.dic['studydate'] = studydate
        self.dic['studytime'] = studytime
        self.dic['studydescription'] = studydescription
        self.dic['patientid'] = patientid

        return self.dic


class DcmSeries(object):
    def __init__(self):
        self.dic = {'seriesuid': '',
                    'studyuid': '',
                    'modality': '',
                    'seriesnumber': '',
                    'seriesdate': '',
                    'seriestime': '',
                    'seriesdescription': '',
                    'bodypartexamined': '',
                    'patientposition': '',
                    'manufacturer': '',
                    'institutionname': '',
                    'seriespixeldatafilepath': '',
                    }

    # don't have pid, studyuid, updatetime, seriespixeldatafilepath
    def get_dicom_series(self, dataset):
        if 'SeriesInstanceUID' in dataset:
            seriesuid = dataset.data_element("SeriesInstanceUID").value
        else:
            seriesuid = ''

        if 'StudyInstanceUID' in dataset:
            studyuid = dataset.data_element('StudyInstanceUID').value
        else:
            studyuid = ''

        if 'Modality' in dataset:
            modality = dataset.data_element("Modality").value
        else:
            modality = ''

        if 'SeriesNumber' in dataset:
            seriesnumber = dataset.data_element("SeriesNumber").value
        else:
            seriesnumber = ''

        if 'SeriesDate' in dataset:
            seriesdate = dataset.data_element("SeriesDate").value
            seriesdate = get_date_time(1, seriesdate)
        else:
            seriesdate = None

        if 'SeriesTime' in dataset:
            seriestime = dataset.data_element("SeriesTime").value
            seriestime = get_date_time(2, seriestime)
        else:
            seriestime = None

        if 'SeriesDescription' in dataset:
            seriesdescription = dataset.data_element("StudyDescription").value
        else:
            seriesdescription = ''

        if 'BodyPartExamined' in dataset:
            bodypartexamined = dataset.data_element("BodyPartExamined").value
        else:
            bodypartexamined = ''

        if 'PatientPosition' in dataset:
            patientposition = dataset.data_element("PatientPosition").value
        else:
            patientposition = ''

        if 'Manufacturer' in dataset:
            manufacturer = dataset.data_element("Manufacturer").value
        else:
            manufacturer = ''

        if 'InstitutionName' in dataset:
            institutionname = dataset.data_element("InstitutionName").value
        else:
            institutionname = ''

        self.dic['seriesuid'] = seriesuid
        self.dic['studyuid'] = studyuid
        self.dic['modality'] = modality
        self.dic['seriesnumber'] = seriesnumber
        self.dic['seriesdate'] = seriesdate
        self.dic['seriestime'] = seriestime
        self.dic['seriesdescription'] = seriesdescription
        self.dic['bodypartexamined'] = bodypartexamined
        self.dic['patientposition'] = patientposition
        self.dic['manufacturer'] = manufacturer
        self.dic['institutionname'] = institutionname

        return self.dic

    def get_series_uid(self, ds):
        series_uid = None
        if 'SeriesInstanceUID' in ds:
            series_uid = ds.data_element("SeriesInstanceUID").value
        return series_uid

    def get_image_uid(self, ds):
        image_uid = None
        if 'SOPInstanceUID' in ds:
            image_uid = ds.data_element("SOPInstanceUID").value
        return image_uid


class DcmImage(object):
    def __init__(self):
        self.dic = {'imageuid': '',
                    'seriesuid': '',
                    'instancenumber': '',
                    'patientorientation': '',
                    'acquisitiondatetime': '',
                    'rowcount': '',
                    'columncount': '',
                    'bitsallocated': '',
                    'bitsstored': '',
                    'highbit': '',
                    'windowwidth': '',
                    'windowcenter': '',
                    'imagecomments': '',
                    'lossyimagecompression': '',
                    'lossyimagecompressionratio': '',
                    'pixelspacing': '',
                    'imageorientationpatient': '',
                    'imagepositionpatient': '',
                    'slicethickness': '',
                    'slicelocation': '',
                    'samplesperpixel': '',
                    'rescaleintercept': '',
                    'rescaleslope': '',
                    'rescaletype': '',
                    'dcmfilepath': '',
                    'updatetime': '',
                    }

    # don't have pid, seriesuid, updatetime, dcmfilepath
    def get_dicom_image(self, dataset):
        if 'SOPInstanceUID' in dataset:
            imageuid = dataset.data_element("SOPInstanceUID").value
        else:
            imageuid = None

        if 'SeriesInstanceUID' in dataset:
            seriesuid = dataset.data_element("SeriesInstanceUID").value
        else:
            seriesuid = None

        if 'InstanceNumber' in dataset:
            instancenumber = dataset.data_element("InstanceNumber").value
        else:
            instancenumber = None

        if 'PatientOrientation' in dataset:
            patientorientation = str(dataset.data_element("PatientOrientation").value)
        else:
            patientorientation = ''

        if 'AcquisitionDateTime' in dataset:
            acquisitiondatetime = dataset.data_element("AcquisitionDateTime").value
            acquisitiondatetime = get_date_time(3, acquisitiondatetime)
        else:
            acquisitiondatetime = None

        if 'Rows' in dataset:
            rowcount = dataset.data_element("Rows").value
        else:
            rowcount = ''

        if 'Columns' in dataset:
            columncount = dataset.data_element("Columns").value
        else:
            columncount = ''

        if 'BitsAllocated' in dataset:
            bitsallocated = dataset.data_element("BitsAllocated").value
        else:
            bitsallocated = ''

        if 'BitsStored' in dataset:
            bitsstored = dataset.data_element("BitsStored").value
        else:
            bitsstored = ''

        if 'HighBit' in dataset:
            highbit = dataset.data_element("HighBit").value
        else:
            highbit = ''

        if 'WindowWidth' in dataset:
            windowwidth = str(dataset.data_element("WindowWidth").value)
        else:
            windowwidth = ''

        if 'WindowCenter' in dataset:
            windowcenter = str(dataset.data_element("WindowCenter").value)
        else:
            windowcenter = ''

        if 'ImageComments' in dataset:
            imagecomments = dataset.data_element("ImageComments").value
        else:
            imagecomments = ''

        if 'LossyImageCompression' in dataset:
            lossyimagecompression = dataset.data_element("LossyImageCompression").value
        else:
            lossyimagecompression = ''

        if 'LossyImageCompressionRatio' in dataset:
            lossyimagecompressionratio = dataset.data_element("LossyImageCompressionRatio").value
        else:
            lossyimagecompressionratio = ''

        if 'PixelSpacing' in dataset:
            pixelspacing = str(dataset.data_element("PixelSpacing").value)
        else:
            pixelspacing = None

        if 'ImageOrientationPatient' in dataset:
            imageorientationpatient = str(dataset.data_element("ImageOrientationPatient").value)
        else:
            imageorientationpatient = None

        if 'ImagePositionPatient' in dataset:
            imagepositionpatient = str(dataset.data_element("ImagePositionPatient").value)
        else:
            imagepositionpatient = None

        if 'SliceThickness' in dataset:
            slicethickness = dataset.data_element("SliceThickness").value
        else:
            slicethickness = ''

        if 'SliceLocation' in dataset:
            slicelocation = dataset.data_element("SliceLocation").value
        else:
            slicelocation = ''

        if 'SamplesPerPixel' in dataset:
            samplesperpixel = dataset.data_element("SamplesPerPixel").value
        else:
            samplesperpixel = ''

        if 'RescaleIntercept' in dataset:
            rescaleintercept = dataset.data_element("RescaleIntercept").value
        else:
            rescaleintercept = None

        if 'RescaleSlope' in dataset:
            rescaleslope = dataset.data_element("RescaleSlope").value
        else:
            rescaleslope = ''

        if 'RescaleType' in dataset:
            rescaletype = dataset.data_element("RescaleType").value
        else:
            rescaletype = ''

        self.dic['imageuid'] = imageuid
        self.dic['seriesuid'] = seriesuid
        self.dic['instancenumber'] = instancenumber
        self.dic['patientorientation'] = patientorientation
        self.dic['acquisitiondatetime'] = acquisitiondatetime
        self.dic['rowcount'] = rowcount
        self.dic['columncount'] = columncount
        self.dic['bitsallocated'] = bitsallocated
        self.dic['bitsstored'] = bitsstored
        self.dic['highbit'] = highbit
        self.dic['windowwidth'] = windowwidth
        self.dic['windowcenter'] = windowcenter
        self.dic['imagecomments'] = imagecomments
        self.dic['lossyimagecompression'] = lossyimagecompression
        self.dic['lossyimagecompressionratio'] = lossyimagecompressionratio
        self.dic['pixelspacing'] = pixelspacing
        self.dic['imageorientationpatient'] = imageorientationpatient
        self.dic['imagepositionpatient'] = imagepositionpatient
        self.dic['slicethickness'] = slicethickness
        self.dic['slicelocation'] = slicelocation
        self.dic['samplesperpixel'] = samplesperpixel
        self.dic['rescaleintercept'] = rescaleintercept
        self.dic['rescaleslope'] = rescaleslope
        self.dic['rescaletype'] = rescaletype

        return self.dic


if __name__ == '__main__':
    filename = "D:/dcm/c_get/1.3.12.2.1107.5.1.4.64606.30000018071420505204600015281/" \
               "CT.1.3.12.2.1107.5.1.4.64606.30000018071420505204600015282.dcm"
    dataset = pydicom.dcmread(filename)

    a = DcmPatient()
    patient_dic = a.get_dicom_patient(dataset)
    print patient_dic

    b = DcmStudy()
    study_dic = b.get_dicom_study(dataset)
    print study_dic

    c = DcmSeries()
    series_dic = c.get_dicom_series(dataset)
    print series_dic

    d = DcmImage()
    image_dic = d.get_dicom_image(dataset)
    print image_dic
