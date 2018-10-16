# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models

# Create your models here.


class Patient(models.Model):
    pid = models.AutoField(primary_key=True)
    patientid = models.CharField(unique=True, max_length=64)
    patientname = models.CharField(max_length=64, blank=True)
    patientbirthdate = models.DateField(blank=True, null=True)
    patientsex = models.CharField(max_length=16, blank=True)
    patientsize = models.CharField(max_length=16, blank=True)
    patientweight = models.CharField(max_length=16, blank=True)
    patientcomments = models.CharField(max_length=1024, blank=True)
    importdatetime = models.DateTimeField(blank=True)
    updatetime = models.DateTimeField(auto_now=True)

    class Meta:
        managed = False
        db_table = 'patient'


class Series(models.Model):

    WHETHER_BUILD = (
        (0, 'not_build'),
        (1, 'builded')
    )
    pid = models.AutoField(primary_key=True)
    seriesuid = models.CharField(unique=True, max_length=64)
    buildvolumesign = models.IntegerField(choices=WHETHER_BUILD, default=1)
    studyuid = models.ForeignKey('Study', on_delete=models.CASCADE, db_column='studyuid', blank=True, null=True,
                                 related_name='sub2', to_field='studyuid')
    modality = models.CharField(max_length=16)
    seriesnumber = models.IntegerField(blank=True, null=True)
    seriesdate = models.DateField(blank=True, null=True)
    seriestime = models.TimeField(blank=True, null=True)
    seriesdescription = models.CharField(max_length=64, blank=True, null=True)
    bodypartexamined = models.CharField(max_length=16, blank=True, null=True)
    patientposition = models.CharField(max_length=16, blank=True, null=True)
    manufacturer = models.CharField(max_length=64, blank=True, null=True)
    institutionname = models.CharField(max_length=64, blank=True, null=True)
    seriespixeldatafilepath = models.CharField(max_length=255, blank=True, null=True)
    updatetime = models.DateTimeField(auto_now=True)

    class Meta:
        managed = False
        db_table = 'series'


class Study(models.Model):
    pid = models.AutoField(primary_key=True)
    studyuid = models.CharField(unique=True, max_length=64)
    patientid = models.ForeignKey('Patient', on_delete=models.CASCADE, related_name='sub1', db_column='patientid',
                                  to_field='patientid')
    studyid = models.CharField(max_length=16, blank=True, null=True)
    studydate = models.DateField(blank=True, null=True)
    studytime = models.TimeField(blank=True, null=True)
    studydescription = models.CharField(max_length=1024, blank=True, null=True)
    updatetime = models.DateTimeField(auto_now=True)

    class Meta:
        managed = False
        db_table = 'study'


class Image(models.Model):
    WHETHER_UPDATE = (
        (0, 'not_update'),
        (1, 'updates')
    )

    pid = models.AutoField(primary_key=True)
    imageuid = models.CharField(unique=True, max_length=64)
    updatesign = models.IntegerField(choices=WHETHER_UPDATE, default=0)
    seriesuid = models.ForeignKey('Series', on_delete=models.CASCADE, db_column='seriesuid', related_name='sub3',
                                  to_field='seriesuid')
    instancenumber = models.IntegerField(blank=True, null=True)
    patientorientation = models.CharField(max_length=33, blank=True, null=True)
    acquisitiondatetime = models.DateTimeField(blank=True, null=True)
    rowcount = models.IntegerField(blank=True, null=True)
    columncount = models.IntegerField(blank=True, null=True)
    bitsallocated = models.IntegerField(blank=True, null=True)
    bitsstored = models.IntegerField(blank=True, null=True)
    highbit = models.IntegerField(blank=True, null=True)
    windowwidth = models.CharField(max_length=135, blank=True, null=True)
    windowcenter = models.CharField(max_length=135, blank=True, null=True)
    imagecomments = models.CharField(max_length=10240, blank=True, null=True)
    lossyimagecompression = models.CharField(max_length=16, blank=True, null=True)
    lossyimagecompressionratio = models.CharField(max_length=271, blank=True, null=True)
    pixelspacing = models.CharField(max_length=64, blank=True, null=True)
    imageorientationpatient = models.CharField(max_length=101, blank=True, null=True)
    imagepositionpatient = models.CharField(max_length=50, blank=True, null=True)
    slicethickness = models.CharField(max_length=16, blank=True, null=True)
    slicelocation = models.CharField(max_length=16, blank=True, null=True)
    samplesperpixel = models.IntegerField(blank=True, null=True)
    rescaleintercept = models.CharField(max_length=16, blank=True, null=True)
    rescaleslope = models.CharField(max_length=16, blank=True, null=True)
    rescaletype = models.CharField(max_length=16, blank=True, null=True)
    dcmfilepath = models.CharField(max_length=255, blank=True, null=True)
    updatetime = models.DateTimeField(auto_now=True)

    class Meta:
        managed = False
        db_table = 'image'


class GraphElement(models.Model):
    graphelementuid = models.AutoField(primary_key=True)
    imageuid = models.ForeignKey('Image', on_delete=models.CASCADE, db_column='imageuid', to_field='imageuid')
    dotproductpath = models.CharField(max_length=255, blank=True, null=True)
    importdatatime = models.DateTimeField(auto_now_add=True)
    updatetime = models.DateTimeField(auto_now=True)

    class Meta:
        managed = False
        db_table = 'graph_element'
