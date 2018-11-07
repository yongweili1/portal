# -*-coding:utf-8-*-
from django.conf.urls import url

from . import views

urlpatterns = [
    url(r'^roidata/$', views.RoiAPIView.as_view(), name='roidata'),
]
