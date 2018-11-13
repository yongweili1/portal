from rest_framework import serializers
from rest_framework.serializers import ModelSerializer

from models import Roi, Series


class ROISerializer(ModelSerializer):
    seriesuid = serializers.SlugRelatedField(queryset=Series.objects.all(), slug_field='seriesuid')

    class Meta:
        model = Roi
        fields = '__all__'
