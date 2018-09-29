# -*-coding:utf-8-*-
from rest_framework.serializers import ModelSerializer, Serializer

from patientinformations.models import Image


class UpdatePatientSerializer(Serializer):
    pass


class UpdateImageSerializer(ModelSerializer):

    class Meta:
        model = Image
        fields = '__all__'

    def update(self, instance, validated_data):
        instance.imageuid = validated_data.get('imageuid', instance.imageuid)
        instance.updatesign = validated_data.get('updatesign', instance.updatesign)
        instance.seriesuid = validated_data.get('seriesuid', instance.seriesuid)
        instance.instancenumber = validated_data.get('instancenumber', instance.instancenumber)
        instance.patientorientation = validated_data.get('patientorientation', instance.patientorientation)
        instance.acquisitiondatetime = validated_data.get('acquisitiondatetime', instance.acquisitiondatetime)
        instance.rowcount = validated_data.get('rowcount', instance.rowcount)
        instance.columncount = validated_data.get('columncount', instance.columncount)
        instance.bitsallocated = validated_data.get('bitsallocated', instance.bitsallocated)
        instance.bitsstored = validated_data.get('bitsstored', instance.bitsstored)
        instance.highbit = validated_data.get('highbit', instance.highbit)
        instance.windowwidth = validated_data.get('windowwidth', instance.windowwidth)
        instance.windowcenter = validated_data.get('windowcenter', instance.windowcenter)
        instance.imagecomments = validated_data.get('imagecomments', instance.imagecomments)
        instance.lossyimagecompression = validated_data.get('lossyimagecompression', instance.lossyimagecompression)
        instance.lossyimagecompressionratio = validated_data.get('lossyimagecompressionratio', instance.lossyimagecompressionratio)
        instance.pixelspacing = validated_data.get('pixelspacing', instance.pixelspacing)
        instance.imageorientationpatient = validated_data.get('imageorientationpatient', instance.imageorientationpatient)
        instance.imagepositionpatient = validated_data.get('imagepositionpatient', instance.imagepositionpatient)
        instance.slicethickness = validated_data.get('slicethickness', instance.slicethickness)
        instance.slicelocation = validated_data.get('slicelocation', instance.slicelocation)
        instance.samplesperpixel = validated_data.get('samplesperpixel', instance.samplesperpixel)
        instance.rescaleintercept = validated_data.get('rescaleintercept', instance.rescaleintercept)
        instance.rescaleslope = validated_data.get('rescaleslope', instance.rescaleslope)
        instance.rescaletype = validated_data.get('rescaletype', instance.rescaletype)
        instance.dcmfilepath = validated_data.get('dcmfilepath', instance.dcmfilepath)

        instance.save()
        return instance