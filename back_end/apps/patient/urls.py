# -*-coding:utf-8-*-

from django.conf.urls import url
from django.views.decorators.csrf import csrf_exempt

from . import views

urlpatterns = [
    url(r'^patinfolist/$', csrf_exempt(views.Patient.as_view()), name='patinfolist'),
    # url(r'^websocket', views.websocket, name='websocket'),
]

# router = DefaultRouter()
# router.register(r'perinfoviewset', views.PerInfoViewSet)
# urlpatterns += router.urls
