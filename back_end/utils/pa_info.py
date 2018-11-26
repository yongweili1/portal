# -*-coding:utf-8-*-
import copy
from db_context.models import Study, Series, Patient, Image


class PaInfo(object):

    def __init__(self):
        self.patinfos_list = []
        self.series_list = []
        self.patinfo_dict = {
            'patientId': '',
            'patientName': '',
            'gender': '',
            'seriesInfo': [],
        }
        self.series_dict = {
            'seriesuid': '',
            'imagequantity': '',
            'seriesdescription': ''
        }

    def get_infolist(self):
        """
        Provide patients information list
        :return:patinfos_list:information list
        """

        queryset = Patient.objects.all()

        if len(queryset) != 0:
            for pat in queryset:
                # 深拷贝字典，因为字典创建在循环外部，要避免重复赋值时覆盖问题
                pat_dict = copy.deepcopy(self.patinfo_dict)
                pat_dict['patientId'] = pat.patientid
                pat_dict['patientName'] = pat.patientname
                pat_dict['gender'] = pat.patientsex

                stu = Study.objects.filter(patientid=pat.patientid)

                ser_list = copy.deepcopy(self.series_list)

                if len(stu) != 0:
                    for st in stu:
                        ser = Series.objects.filter(studyuid=st.studyuid)

                        if len(ser) != 0:
                            for se in ser:
                                series_dict = copy.deepcopy(self.series_dict)
                                series_dict['seriesuid'] = se.seriesuid
                                image_number = Image.objects.filter(seriesuid=se.seriesuid).count()
                                series_dict['imagequantity'] = str(image_number)
                                series_dict['seriesdescription'] = se.seriesdescription
                                ser_list.append(series_dict)
                            pat_dict['seriesInfo'] = ser_list

                        else:
                            pat_dict['seriesInfo'] = ser_list

                else:
                    pat_dict['seriesInfo'] = ser_list

                self.patinfos_list.append(pat_dict)

        return self.patinfos_list

    def delete_info(self, patientidarray, studyidarray, seriesidarray):
        """
        delete information
        :param patientidarray:array of patientid
        :param studyidarray: array of studyuid
        :param seriesidarray: array of seriesid
        :return:
        """
        if isinstance(seriesidarray, list):
            for seriesId in seriesidarray:
                Series.objects.filter(seriesuid=seriesId).delete()
        else:
            Series.objects.filter(seriesuid=seriesidarray).delete()
        if isinstance(studyidarray, list):
            for studyId in studyidarray:
                Study.objects.filter(studyuid=studyId).delete()
        else:
            Study.objects.filter(studyuid=studyidarray).delete()
        if isinstance(patientidarray, list):
            for patientId in patientidarray:
                Patient.objects.filter(patientid=patientId).delete()
        else:
            Patient.objects.filter(patientid=patientidarray).delete()
