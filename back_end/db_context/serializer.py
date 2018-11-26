# -*-coding:utf-8-*-
from rest_framework import serializers
from rest_framework.serializers import ModelSerializer

from models import Patient, Image, Study, Series, Roi, Contour, Script


class PerInfoSerializer(ModelSerializer):
    class Meta:
        model = Patient
        fields = '__all__'


class StudySerializer(ModelSerializer):
    patientid = serializers.SlugRelatedField(queryset=Patient.objects.all(), slug_field='patientid')

    class Meta:
        model = Study
        fields = '__all__'


class SeriesSerializer(ModelSerializer):
    studyuid = serializers.SlugRelatedField(queryset=Study.objects.all(), slug_field='studyuid')

    class Meta:
        model = Series
        fields = '__all__'


class ImageSerializer(ModelSerializer):
    seriesuid = serializers.SlugRelatedField(queryset=Series.objects.all(), slug_field='seriesuid')

    class Meta:
        model = Image
        fields = '__all__'


class RoiSerializer(ModelSerializer):
    seriesuid = serializers.SlugRelatedField(queryset=Series.objects.all(), slug_field='seriesuid')

    class Meta:
        model = Roi
        fields = '__all__'


class ContourSerializer(ModelSerializer):
    roiuid = serializers.SlugRelatedField(queryset=Roi.objects.all(), slug_field='roiuid')

    class Meta:
        model = Contour
        fields = '__all__'


class ScriptSerializer(ModelSerializer):
    class Meta:
        model = Script
        fields = '__all__'