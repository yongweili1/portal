# -*-coding:utf-8-*-
from django.conf.urls import url
from django.views.decorators.csrf import csrf_exempt

from . import views

urlpatterns = [
    url(r'^pacspatient/$', views.GetPatient.as_view(), name='pacspatient'),
    url(r'^pacsseries/$', views.DownloadSeries.as_view(), name='pacsseries'),
]
