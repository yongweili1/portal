# -*- coding: utf-8 -*-
# Generated by Django 1.11.15 on 2018-10-18 15:04
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Image',
            fields=[
                ('pid', models.AutoField(primary_key=True, serialize=False)),
                ('imageuid', models.CharField(max_length=64, unique=True)),
                ('updatesign', models.IntegerField(choices=[(0, b'not_update'), (1, b'updates')], default=0)),
                ('instancenumber', models.IntegerField(blank=True, null=True)),
                ('patientorientation', models.CharField(blank=True, max_length=33, null=True)),
                ('acquisitiondatetime', models.DateTimeField(blank=True, null=True)),
                ('rowcount', models.IntegerField(blank=True, null=True)),
                ('columncount', models.IntegerField(blank=True, null=True)),
                ('bitsallocated', models.IntegerField(blank=True, null=True)),
                ('bitsstored', models.IntegerField(blank=True, null=True)),
                ('highbit', models.IntegerField(blank=True, null=True)),
                ('windowwidth', models.CharField(blank=True, max_length=135, null=True)),
                ('windowcenter', models.CharField(blank=True, max_length=135, null=True)),
                ('imagecomments', models.CharField(blank=True, max_length=10240, null=True)),
                ('lossyimagecompression', models.CharField(blank=True, max_length=16, null=True)),
                ('lossyimagecompressionratio', models.CharField(blank=True, max_length=271, null=True)),
                ('pixelspacing', models.CharField(blank=True, max_length=64, null=True)),
                ('imageorientationpatient', models.CharField(blank=True, max_length=101, null=True)),
                ('imagepositionpatient', models.CharField(blank=True, max_length=50, null=True)),
                ('slicethickness', models.CharField(blank=True, max_length=16, null=True)),
                ('slicelocation', models.CharField(blank=True, max_length=16, null=True)),
                ('samplesperpixel', models.IntegerField(blank=True, null=True)),
                ('rescaleintercept', models.CharField(blank=True, max_length=16, null=True)),
                ('rescaleslope', models.CharField(blank=True, max_length=16, null=True)),
                ('rescaletype', models.CharField(blank=True, max_length=16, null=True)),
                ('dcmfilepath', models.CharField(blank=True, max_length=255, null=True)),
                ('updatetime', models.DateTimeField(auto_now=True)),
            ],
            options={
                'db_table': 'image',
                'managed': False,
            },
        ),
        migrations.CreateModel(
            name='Patient',
            fields=[
                ('pid', models.AutoField(primary_key=True, serialize=False)),
                ('patientid', models.CharField(max_length=64, unique=True)),
                ('patientname', models.CharField(blank=True, max_length=64)),
                ('patientbirthdate', models.DateField(blank=True, null=True)),
                ('patientsex', models.CharField(blank=True, max_length=16)),
                ('patientsize', models.CharField(blank=True, max_length=16)),
                ('patientweight', models.CharField(blank=True, max_length=16)),
                ('patientcomments', models.CharField(blank=True, max_length=1024)),
                ('importdatetime', models.DateTimeField(blank=True)),
                ('updatetime', models.DateTimeField(auto_now=True)),
            ],
            options={
                'db_table': 'patient',
                'managed': False,
            },
        ),
        migrations.CreateModel(
            name='Script',
            fields=[
                ('pid', models.AutoField(primary_key=True, serialize=False)),
                ('scriptname', models.CharField(max_length=64, unique=True)),
                ('userid', models.CharField(max_length=64)),
                ('scriptpath', models.CharField(blank=True, max_length=255, null=True)),
                ('importdatatime', models.DateTimeField(auto_now_add=True)),
                ('updatetime', models.DateTimeField(auto_now=True)),
            ],
            options={
                'db_table': 'script',
                'managed': False,
            },
        ),
        migrations.CreateModel(
            name='Series',
            fields=[
                ('pid', models.AutoField(primary_key=True, serialize=False)),
                ('seriesuid', models.CharField(max_length=64, unique=True)),
                ('buildvolumesign', models.IntegerField(choices=[(0, b'need_build'), (1, b'builded')], default=1)),
                ('modality', models.CharField(max_length=16)),
                ('seriesnumber', models.IntegerField(blank=True, null=True)),
                ('seriesdate', models.DateField(blank=True, null=True)),
                ('seriestime', models.TimeField(blank=True, null=True)),
                ('seriesdescription', models.CharField(blank=True, max_length=64, null=True)),
                ('bodypartexamined', models.CharField(blank=True, max_length=16, null=True)),
                ('patientposition', models.CharField(blank=True, max_length=16, null=True)),
                ('manufacturer', models.CharField(blank=True, max_length=64, null=True)),
                ('institutionname', models.CharField(blank=True, max_length=64, null=True)),
                ('seriespixeldatafilepath', models.CharField(blank=True, max_length=255, null=True)),
                ('updatetime', models.DateTimeField(auto_now=True)),
            ],
            options={
                'db_table': 'series',
                'managed': False,
            },
        ),
        migrations.CreateModel(
            name='Study',
            fields=[
                ('pid', models.AutoField(primary_key=True, serialize=False)),
                ('studyuid', models.CharField(max_length=64, unique=True)),
                ('studyid', models.CharField(blank=True, max_length=16, null=True)),
                ('studydate', models.DateField(blank=True, null=True)),
                ('studytime', models.TimeField(blank=True, null=True)),
                ('studydescription', models.CharField(blank=True, max_length=1024, null=True)),
                ('updatetime', models.DateTimeField(auto_now=True)),
            ],
            options={
                'db_table': 'study',
                'managed': False,
            },
        ),
    ]
