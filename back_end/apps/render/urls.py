# -*-coding:utf-8-*-

from django.conf.urls import url

from . import views

urlpatterns = [
    url(r'^page/$', views.TurnPage.as_view(), name='page'),
    url(r'^home/$', views.Home.as_view(), name='home'),
    url(r'^volumes/$', views.LoadVolume.as_view(), name='volumes'),
    url(r'^images/$', views.GetImage.as_view(), name='images'),
    url(r'^colors/$', views.ChangeColor.as_view(), name='colors'),
    url(r'^pan/$', views.Pan.as_view(), name='pan'),
    url(r'^roll/$', views.Roll.as_view(), name='roll'),
    url(r'^rotate/$', views.Rotate.as_view(), name='rotate'),
    url(r'^zoom/$', views.Zoom.as_view(), name='zoom'),
    url(r'^serids/$', views.GetSeriesUidList.as_view(), name='serids'),
    url(r'^reset/$', views.ReSetVolume.as_view(), name='reset'),
    url(r'^windows/$', views.ChangeWindow.as_view(), name='windows'),
    url(r'^size/$', views.WindowSize.as_view(), name='size'),
    url(r'^cross/$', views.CrossLineLocation.as_view(), name='cross'),
    url(r'^center/$', views.SetCenter.as_view(), name='center'),
    url(r'^contour/$', views.Contour.as_view(), name='contour'),
    url(r'^roi/$', views.Roi.as_view(), name='roi'),
]
