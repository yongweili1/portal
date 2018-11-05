# -*-coding:utf-8-*-
import copy
from serve.util.models import Study, Series, Patient


class InfoList(object):

    def __init__(self):
        self.patinfos_list = []
        self.patinfo_dict = {
            'patientId': '',
            'patientName': '',
            'gender': '',
            'studyDescription': '',
            'modality': '',
            'studyDate': '',
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

                # 获取study信息的查询集，查询集为对象列表
                stu = Study.objects.filter(patientid=pat.patientid)

                if len(stu) != 0:
                    for st in stu:
                        # 深拷贝字典，因为字典创建在循环外部，要避免重复赋值时覆盖问题
                        pat_dict = copy.deepcopy(pat_dict)
                        pat_dict['studyDescription'] = st.studydescription
                        pat_dict['studyDate'] = st.studydate

                        ser = Series.objects.filter(studyuid=st.studyuid)

                        if len(ser) != 0:
                            for se in ser:
                                pat_dict['modality'] = se.modality
                                self.patinfos_list.append(pat_dict)

                        # 父表study无子表series，添加进列表
                        else:
                            self.patinfos_list.append(pat_dict)

                # 父表patient无子表study，添加进列表
                else:
                    self.patinfos_list.append(pat_dict)

        return self.patinfos_list

    def delete_info(self, patientIdArray, studyIdArray, seriesIdArray):
        """
        Provide patients information list
        :return:patinfos_list:information list
        """
        if isinstance(seriesIdArray, list):
            for seriesId in seriesIdArray:
                Series.objects.filter(seriesuid=seriesId).delete()
        else:
            Series.objects.filter(seriesuid=seriesIdArray).delete()
        if isinstance(seriesIdArray, list):
            for studyId in studyIdArray:
                Study.objects.filter(studyuid=studyId).delete()
        else:
            Study.objects.filter(studyuid=studyIdArray).delete()
        if isinstance(seriesIdArray, list):
            for patientId in patientIdArray:
                Patient.objects.filter(patientid=patientId).delete()
        else:
            Patient.objects.filter(patientid=patientIdArray).delete()
