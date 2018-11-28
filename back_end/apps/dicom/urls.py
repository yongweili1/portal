# -*-coding:utf-8-*-

from django.conf.urls import url
from django.views.decorators.csrf import csrf_exempt

from . import views

urlpatterns = [
    url(r'^upload/$', csrf_exempt(views.Upload.as_view()), name='upload'),
]
