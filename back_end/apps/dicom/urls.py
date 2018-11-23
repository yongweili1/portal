# -*-coding:utf-8-*-

from django.conf.urls import url
from django.views.decorators.csrf import csrf_exempt
from . import views

urlpatterns = [
    url(r'^patinfos/$', csrf_exempt(views.Patinfo.as_view()), name='patinfos'),
]
