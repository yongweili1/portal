# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.apps import AppConfig


class PatientConfig(AppConfig):
    name = 'patient'

    # 直观可读的名字，在admin中会显示
    verbose_name = "患者信息"
