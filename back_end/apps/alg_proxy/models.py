# -*- coding: utf-8 -*-
from __future__ import unicode_literals
from django.db import models

# Create your models here.


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
        managed = False
        db_table = 'alg_csv'
