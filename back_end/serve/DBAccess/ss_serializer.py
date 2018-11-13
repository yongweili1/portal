from rest_framework import serializers
from rest_framework.serializers import ModelSerializer

from models import Series, Roi, Contour


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
