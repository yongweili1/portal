# -*-coding:utf-8-*-
from django.conf.urls import url

from . import views

urlpatterns = [
    url(r'^macro/$', views.MacroRecording.as_view(), name='macro'),
    url(r'^script/$', views.RunSript.as_view(), name='script'),
]
