# -*-coding:utf-8-*-

from django.conf.urls import url

from . import views

urlpatterns = [
    url(r'^algorithms/$', views.LoadAlg.as_view(), name='patinfos'),
    url(r'^results/$', views.GetAlgResult.as_view(), name='results'),
]
