# -*- coding: utf-8 -*-
import datetime
import sys
import os

from django.db import transaction
from rest_framework import serializers
from db_access.models import Patient, Study, Series, Image
from db_access.serializer import PerInfoSerializer, StudySerializer, SeriesSerializer, ImageSerializer
from util.dcm_reader import DcmPatient, DcmStudy, DcmSeries, DcmImage

reload(sys)
sys.setdefaultencoding('utf8')


class UploadDcm(object):

    def upload_dcm(self, datasetlist=None, filepath=None):
        """
        upload dicom data to DB
        :param datasetlist: dicom datasetlist
        :return: None
        """
        # sign = True

        for dataset in datasetlist:

            # 开启事务
            with transaction.atomic():
                # 创建保存点，用于提交和回滚
                save_id = transaction.savepoint()

                try:
                    a = DcmPatient()
                    patient_dic = a.get_dicom_patient(dataset)
                    # print(patient_dic)
                    # 判断数据库中是否存在该对象，如有，更新时间，不执行数据入库
                    if len(Patient.objects.filter(patientid=patient_dic['patientid'])) != 0:
                        Patient.objects.filter(patientid=patient_dic['patientid']).update(
                            updatetime=datetime.datetime.now())
                    else:
                        # 创建序列化器对象，验证信息并保存到数据库
                        ser = PerInfoSerializer(data=patient_dic)
                        # 数据验证失败，回滚到保存点并抛出序列化器验证错误
                        if not ser.is_valid(raise_exception=True):
                            transaction.savepoint_rollback(save_id)
                            raise serializers.ValidationError('patient表数据有误验证失败')
                        ser.save()

                    b = DcmStudy()
                    study_dic = b.get_dicom_study(dataset)
                    # print(study_dic)
                    if len(Study.objects.filter(studyuid=study_dic['studyuid'])) != 0:
                        Study.objects.filter(studyuid=study_dic['studyuid']).update(updatetime=datetime.datetime.now())
                    else:
                        serstudy = StudySerializer(data=study_dic)
                        if not serstudy.is_valid(raise_exception=True):
                            transaction.savepoint_rollback(save_id)
                            raise serializers.ValidationError('study表数据有误验证失败')
                        serstudy.save()

                    c = DcmSeries()
                    series_dic = c.get_dicom_series(dataset)
                    # print(series_dic)
                    if len(Series.objects.filter(seriesuid=series_dic['seriesuid'])) != 0:
                        Series.objects.filter(seriesuid=series_dic['seriesuid']).update(buildvolumesign=int(1),
                                                                                        updatetime=datetime.datetime.now())
                    else:
                        serseries = SeriesSerializer(data=series_dic)
                        if not serseries.is_valid(raise_exception=True):
                            transaction.savepoint_rollback(save_id)
                            raise serializers.ValidationError('series表数据有误验证失败')
                        serseries.save()

                    d = DcmImage()
                    image_dic = d.get_dicom_image(dataset)
                    image_dic['dcmfilepath'] = os.path.join(filepath, image_dic['seriesuid'], image_dic['imageuid'])
                    # print(image_dic)
                    if len(Image.objects.filter(imageuid=image_dic['imageuid'])) != 0:
                        Image.objects.filter(imageuid=image_dic['imageuid']).update(updatesign=int(1),
                                                                                    updatetime=datetime.datetime.now())
                    else:
                        serimage = ImageSerializer(data=image_dic)
                        if not serimage.is_valid(raise_exception=True):
                            transaction.savepoint_rollback(save_id)
                            raise serializers.ValidationError('image表数据有误验证失败')
                        serimage.save()

                except Exception as e:
                    transaction.savepoint_rollback(save_id)
                    raise

                else:
                    transaction.savepoint_commit(save_id)
