from django.conf.urls import url
from django.views.decorators.csrf import csrf_exempt

from . import views

urlpatterns = [
    url(r'^$', views.Greeting.as_view(), name='api'),
    url(r'^load/$', csrf_exempt(views.Load.as_view()), name='load'),
    url(r'^reset/$', csrf_exempt(views.Reset.as_view()), name='reset'),
    url(r'^unload/$', csrf_exempt(views.Unload.as_view()), name='unload'),
    url(r'^show/$', views.Show.as_view(), name='show'),
    url(r'^page/$', views.Page.as_view(), name='page'),
    url(r'^zoom/$', views.Zoom.as_view(), name='zoom'),
    url(r'^rotate/$', views.Rotate.as_view(), name='rotate'),
    url(r'^pan/$', views.Pan.as_view(), name='pan'),
    url(r'^roll/$', views.Roll.as_view(), name='roll'),
    url(r'^color/$', views.Color.as_view(), name='color'),
    url(r'^wcww/$', views.WCWW.as_view(), name='wcww'),
]