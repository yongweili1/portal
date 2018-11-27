# -*-coding:utf-8-*-
from django.conf.urls import url

from . import views

urlpatterns = [
    url(r'^pacspatient/$', views.GetPatient.as_view(), name='pacspatient'),
    url(r'^pacspage/$', views.GetPacsPage.as_view(), name='pacspage'),
    url(r'^pacsseries/$', views.DownloadSeries.as_view(), name='pacsseries'),
]
