# -*-coding:utf-8-*-

from django.conf.urls import url
from django.views.decorators.csrf import csrf_exempt
from rest_framework.routers import DefaultRouter

from . import views

urlpatterns = [
    # url(r'^perinfos/$', views.PerInfoViewSet.as_view({"get": "list"}), name='perinfos'),
    # url(r'^perinfos/(?P<pk>\d+)/$', views.PerInfoViewSet.as_view({"get": "retrieve"}), name='perinfos'),
    url(r'^patinfolist/$', csrf_exempt(views.Patinfolist.as_view()), name='patinfolist'),
    url(r'^getview/$', views.GetView.as_view(), name='getview'),
]


# 视图集中方法繁多，有列表视图和详情视图，采用自动生成路由
router = DefaultRouter()
router.register(r'perinfoviewset', views.PerInfoViewSet)
urlpatterns += router.urls
