# -*-coding:utf-8-*-

from django.conf.urls import url
from django.views.decorators.csrf import csrf_exempt
from . import views

urlpatterns = [
    url(r'^home/$', views.Home.as_view(), name='home'),
    url(r'^volumes/$', views.LoadVolume.as_view(), name='volumes'),
    url(r'^images/$', views.GetImage.as_view(), name='images'),
    url(r'^colors/$', views.ChangeColor.as_view(), name='colors'),
    url(r'^pages/$', views.TurnPage.as_view(), name='pages'),
    url(r'^seats/$', views.Pan.as_view(), name='seats'),
    url(r'^roll/$', views.Roll.as_view(), name='roll'),
    url(r'^rotate/$', views.Rotate.as_view(), name='rotate'),
    url(r'^zooms/$', views.Zoom.as_view(), name='zooms'),
    url(r'^serids/$', views.GetSeriesUidList.as_view(), name='serids'),
    url(r'^backs/$', views.ReSetVolume.as_view(), name='reset'),
    url(r'^windows/$', views.ChangeWindow.as_view(), name='windows'),

    url(r'^haha/$', views.Haha.as_view(), name='HAHA'),
]
