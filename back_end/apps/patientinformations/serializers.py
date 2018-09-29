# -*-coding:utf-8-*-
from rest_framework import serializers
from rest_framework.serializers import ModelSerializer

from patientinformations.models import Patient, Image, Study, Series


# 序列化
class ImageoneSerializer(ModelSerializer):

    class Meta:
        model = Image
        fields = ('imageuid', )


class SeriesoneSerializer(ModelSerializer):

    sub3 = ImageoneSerializer(many=True)

    class Meta:
        model = Series
        fields = ('modality', 'seriesuid', 'studyuid', 'sub3')


class StudyoneSerializer(ModelSerializer):

    sub2 = SeriesoneSerializer(many=True)

    class Meta:
        model = Study
        fields = ('studyid', 'studydate', 'studydescription', 'sub2')


class PerInfooneSerializer(ModelSerializer):

    sub1 = StudyoneSerializer(many=True)

    class Meta:
        model = Patient
        fields = ('patientid', 'patientname', 'patientsex', 'sub1')


# 反序列化

# patient表模型类序列化器，基于模型类将获取到的字典数据写入数据库
class PerInfoSerializer(ModelSerializer):

    class Meta:
        model = Patient
        fields = '__all__'


# study表模型类序列化器
class StudySerializer(ModelSerializer):

    # 外键关联
    patientid = serializers.SlugRelatedField(queryset=Patient.objects.all(), slug_field='patientid')

    class Meta:
        model = Study
        fields = '__all__'


# series表模型类序列化器
class SeriesSerializer(ModelSerializer):

    studyuid = serializers.SlugRelatedField(queryset=Study.objects.all(), slug_field='studyuid')

    class Meta:
        model = Series
        fields = '__all__'


# image表模型类序列化器
class ImageSerializer(ModelSerializer):

    seriesuid = serializers.SlugRelatedField(queryset=Series.objects.all(), slug_field='seriesuid')

    class Meta:
        model = Image
        fields = '__all__'
