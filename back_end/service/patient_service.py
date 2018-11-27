# -*-coding:utf-8-*-

import copy
import datetime
import os

from django.db import transaction

from db_context import patient_ctx, study_ctx, series_ctx, image_ctx
from utils.dcm_reader import DcmPatient, DcmStudy, DcmSeries, DcmImage


class PatientService(object):
    def __init__(self):
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
        :return:patient_list:information list
        """
        patient_list = []

        queryset = patient_ctx.all()
        if len(queryset) == 0:
            return []

        for pat in queryset:
            # 深拷贝字典，因为字典创建在循环外部，要避免重复赋值时覆盖问题
            pat_dict = copy.deepcopy(self.patinfo_dict)
            pat_dict['patientId'] = pat.patientid
            pat_dict['patientName'] = pat.patientname
            pat_dict['gender'] = pat.patientsex

            study_list, msg = study_ctx.retrieve(pat.patientid)

            ser_list = copy.deepcopy(self.series_list)

            if len(study_list) == 0:
                pat_dict['seriesInfo'] = ser_list
                continue

            for st in study_list:
                series_list, msg = series_ctx.retrieve(study_uid=st.studyuid)
                if len(series_list) == 0:
                    pat_dict['seriesInfo'] = ser_list
                    continue
                for se in series_list:
                    series_dict = copy.deepcopy(self.series_dict)
                    series_dict['seriesuid'] = se.seriesuid
                    image_number = image_ctx.count(se.seriesuid)
                    series_dict['imagequantity'] = str(image_number)
                    series_dict['seriesdescription'] = se.seriesdescription
                    ser_list.append(series_dict)
                pat_dict['seriesInfo'] = ser_list

            patient_list.append(pat_dict)

        return patient_list

    def delete_info(self, patientids, studyids, seriesids):
        """
        delete information
        :param patientids:array of patientid
        :param studyids: array of studyuid
        :param seriesids: array of seriesid
        :return:
        """
        series_ctx.delete(seriesids)
        study_ctx.delete(studyids)
        patient_ctx.delete(patientids)

    def upload_dcm(self, datasetlist=None, filepath=None):
        """
        upload dicom data to DB
        :param datasetlist: dicom datasetlist
        :return: None
        """
        if datasetlist is None:
            return False, 'datasetlist is None'

        for dataset in datasetlist:
            # 开启事务
            with transaction.atomic():
                # 创建保存点，用于提交和回滚
                save_point = transaction.savepoint()

                try:
                    success, msg = self.__handle_patient(dataset)
                    if not success:
                        transaction.savepoint_rollback(save_point)

                    success, msg = self.__handle_study(dataset)
                    if not success:
                        transaction.savepoint_rollback(save_point)

                    success, msg = self.__handle_series(dataset)
                    if not success:
                        transaction.savepoint_rollback(save_point)

                    success, msg = self.__handle_image(dataset, filepath)
                    if not success:
                        transaction.savepoint_rollback(save_point)

                except Exception as e:
                    transaction.savepoint_rollback(save_point)
                    raise

                else:
                    transaction.savepoint_commit(save_point)

    def __handle_patient(self, dataset):
        a = DcmPatient()
        patient_dic = a.get_dicom_patient(dataset)
        patientid = patient_dic['patientid']
        if patient_ctx.exist(patientid):
            success, msg = patient_ctx.update(patientid, {'updatetime': datetime.datetime.now()})
        else:
            success, msg = patient_ctx.create(patient_dic)
        return success, msg

    def __handle_study(self, dataset):
        b = DcmStudy()
        study_dic = b.get_dicom_study(dataset)
        studyuid = study_dic['studyuid']
        if study_ctx.exist(studyuid):
            success, msg = study_ctx.update(studyuid, {'updatetime': datetime.datetime.now()})
        else:
            success, msg = study_ctx.create(study_dic)
        return success, msg

    def __handle_series(self, dataset):
        c = DcmSeries()
        series_dic = c.get_dicom_series(dataset)
        seriesuid = series_dic['seriesuid']
        if series_ctx.exist(seriesuid):
            success, msg = series_ctx.update(seriesuid, {'buildvolumesign': 1, 'updatetime': datetime.datetime.now()})
        else:
            success, msg = series_ctx.create(series_dic)
        return success, msg

    def __handle_image(self, dataset, filepath):
        d = DcmImage()
        image_dic = d.get_dicom_image(dataset)
        imageuid = image_dic['imageuid']
        image_dic['dcmfilepath'] = os.path.join(filepath, image_dic['seriesuid'], image_dic['imageuid'])

        if image_ctx.exist(imageuid):
            success, msg = image_ctx.update(imageuid, {'updatesign': 1, 'updatetime': datetime.datetime.now()})
        else:
            success, msg = image_ctx.create(image_dic)
        return success, msg
