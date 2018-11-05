# # -*- coding: utf-8 -*-
# from __future__ import unicode_literals
# # Create your models here.

from django.db import models


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
        app_label = 'Patient'
        managed = False
        db_table = 'patient'

    def __unicode__(self):
        return self.patientname


class Study(models.Model):
    pid = models.AutoField(primary_key=True)
    studyuid = models.CharField(unique=True, max_length=64)
    patientid = models.ForeignKey(Patient, on_delete=models.CASCADE, related_name='sub1', db_column='patientid',
                                  to_field='patientid')
    studyid = models.CharField(max_length=16, blank=True, null=True)
    studydate = models.DateField(blank=True, null=True)
    studytime = models.TimeField(blank=True, null=True)
    studydescription = models.CharField(max_length=1024, blank=True, null=True)
    updatetime = models.DateTimeField(auto_now=True)

    class Meta:
        app_label = 'Study'
        managed = False
        db_table = 'study'


class Series(models.Model):
    WHETHER_BUILD = (
        (0, 'need_build'),
        (1, 'builded')
    )
    pid = models.AutoField(primary_key=True)
    seriesuid = models.CharField(unique=True, max_length=64)
    buildvolumesign = models.IntegerField(choices=WHETHER_BUILD, default=1)
    studyuid = models.ForeignKey(Study, on_delete=models.CASCADE, db_column='studyuid', blank=True, null=True,
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
        app_label = 'Series'
        managed = False
        db_table = 'series'


class Image(models.Model):
    WHETHER_UPDATE = (
        (0, 'not_update'),
        (1, 'updates')
    )

    pid = models.AutoField(primary_key=True)
    imageuid = models.CharField(unique=True, max_length=64)
    updatesign = models.IntegerField(choices=WHETHER_UPDATE, default=0)
    seriesuid = models.ForeignKey(Series, on_delete=models.CASCADE, db_column='seriesuid', related_name='sub3',
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
        app_label = 'Image'
        managed = False
        db_table = 'image'


class Script(models.Model):
    pid = models.AutoField(primary_key=True)
    scriptname = models.CharField(unique=True, max_length=64)
    # userid = models.ForeignKey(User, on_delete=models.CASCADE, db_column='userid', to_field='userid')
    userid = models.CharField(max_length=64)
    scriptpath = models.CharField(max_length=255, blank=True, null=True)
    importdatatime = models.DateTimeField(auto_now_add=True)
    updatetime = models.DateTimeField(auto_now=True)

    class Meta:
        app_label = 'Script'
        managed = False
        db_table = 'script'


class NewDjangoSession(models.Model):
    client_ip = models.CharField(max_length=64)
    session_key = models.CharField(primary_key=True, max_length=40)
    session_data = models.CharField(max_length=255)
    expire_date = models.DateTimeField(blank=True, null=True)

    class Meta:
        app_label = 'NewDjangoSession'
        managed = False
        db_table = 'new_django_session'


class Contours(models.Model):
    uid = models.AutoField(primary_key=True)
    seriesuid = models.ForeignKey(Series, on_delete=models.CASCADE, db_column='seriesuid', to_field='seriesuid')
    cpspath = models.CharField(max_length=255, blank=True, null=True)
    organ = models.CharField(max_length=255, blank=True, null=True)
    patientposition_z = models.FloatField(blank=True, null=True)
    importdatatime = models.DateTimeField(blank=True, null=True)
    updatetime = models.DateTimeField(auto_now=True)

    class Meta:
        app_label = 'Contours'
        managed = False
        db_table = 'contours'


class AlgCsv(models.Model):
    pid = models.AutoField(primary_key=True)
    image_path = models.CharField(unique=True, max_length=64)
    x = models.FloatField()
    y = models.FloatField()
    z = models.IntegerField()
    width = models.FloatField()
    height = models.FloatField()
    depth = models.IntegerField()
    probability = models.IntegerField()
    classname = models.CharField(max_length=64)
    location = models.CharField(max_length=64)

    class Meta:
        app_label = 'AlgCsv'
        managed = False
        db_table = 'alg_csv'


class DjangoSession(models.Model):
    session_key = models.CharField(primary_key=True, max_length=40)
    session_data = models.TextField()
    expire_date = models.DateTimeField()

    class Meta:
        app_label = 'DjangoSession'
        managed = False
        db_table = 'django_session'
