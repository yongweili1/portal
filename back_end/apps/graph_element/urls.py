# -*-coding:utf-8-*-
from django.conf.urls import url
from django.views.decorators.csrf import csrf_exempt
from . import views
from . import views

urlpatterns = [
   url(r'^contour/$', views.GraphElement.as_view(), name='contour'),
]
