# -*-coding:utf-8-*-
from django.contrib.sessions.base_session import AbstractBaseSession
from django.db import models


class NewDjangoSession(models.Model):
    client_ip = models.CharField(max_length=64)
    session_key = models.CharField(primary_key=True, max_length=40)
    session_data = models.CharField(max_length=255)
    expire_date = models.DateTimeField()

    class Meta:
        managed = False
        db_table = 'new_django_session'


class DjangoSession(models.Model):
    session_key = models.CharField(primary_key=True, max_length=40)
    session_data = models.TextField()
    expire_date = models.DateTimeField()

    class Meta:
        managed = False
        db_table = 'django_session'
