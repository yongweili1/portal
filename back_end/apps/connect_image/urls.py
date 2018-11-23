# -*-coding:utf-8-*-

from django.conf.urls import url
from django.views.decorators.csrf import csrf_exempt
from . import views

urlpatterns = [
    url(r'^pages/$', views.TurnPage.as_view(), name='pages'),
    url(r'^home/$', views.Home.as_view(), name='home'),
    url(r'^macro/$', views.MacroRecording.as_view(), name='macro'),
    url(r'^volumes/$', views.LoadVolume.as_view(), name='volumes'),
    url(r'^images/$', views.GetImage.as_view(), name='images'),
    url(r'^colors/$', views.ChangeColor.as_view(), name='colors'),
    url(r'^seats/$', views.Pan.as_view(), name='seats'),
    url(r'^roll/$', views.Roll.as_view(), name='roll'),
    url(r'^rotate/$', views.Rotate.as_view(), name='rotate'),
    url(r'^zooms/$', views.Zoom.as_view(), name='zooms'),
    url(r'^serids/$', views.GetSeriesUidList.as_view(), name='serids'),
    url(r'^backs/$', views.ReSetVolume.as_view(), name='reset'),
    url(r'^windows/$', views.ChangeWindow.as_view(), name='windows'),
    url(r'^size/$', views.WindowSize.as_view(), name='size'),
    url(r'^cross/$', views.CrossLineLocation.as_view(), name='cross'),
    url(r'^script/$', views.RunSript.as_view(), name='script'),
    url(r'^center/$', views.SetCenter.as_view(), name='center'),
    url(r'^contour/$', views.GraphElement.as_view(), name='contour'),
    url(r'^roidata/$', views.RoiAPIView.as_view(), name='roidata'),
]
