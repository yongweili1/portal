# -*- coding: utf-8 -*-
"""back_end URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/1.11/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  url(r'^$', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  url(r'^$', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.conf.urls import url, include
    2. Add a URL to urlpatterns:  url(r'^blog/', include('blog.urls'))
"""
from django.conf.urls import url, include
from django.contrib import admin
from rest_framework.schemas import get_schema_view

# from rest_framework_swagger.renderers import OpenAPIRenderer, SwaggerUIRenderer

# schema_view = get_schema_view(title='我的API',
#                               renderer_classes=[OpenAPIRenderer, SwaggerUIRenderer])

urlpatterns = [
    url(r'^image/', include('connect_image.urls')),
    url(r'^admin/', admin.site.urls),
    # url(r'^docs/', schema_view),
    url(r'^patientinformations/', include('patientinformations.urls')),
    url(r'^dicom/', include('upload_dcm.urls')),
    url(r'^algproxy/', include('alg_proxy.urls')),
    url(r'^pacs/', include('handle_pacs.urls')),
    url(r'^contour/', include('graph_element.urls')),
]
