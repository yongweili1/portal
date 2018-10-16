# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.shortcuts import render

# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView


class GraphElement(APIView):
    def post(self, request):
        type = request.POST.get('type', None)
        dotposition = request.POST.get('dotposition', None)
        if type is None or dotposition is None:
            return Response('参数不完整')
        