# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.http import HttpResponse
from rest_framework.generics import GenericAPIView
from rest_framework.response import Response
from rest_framework.viewsets import ModelViewSet

from InformationList import InfoList
from patientinformations.models import Patient, Image
from patientinformations.serializers import PerInfooneSerializer
from django.forms.models import model_to_dict
from back_end.util.setPage import SetPaginationInit


class PerInfoViewSet(ModelViewSet):
    # 仅提供序列化器和查询集，具体实现方法见底层封装
    queryset = Patient.objects.all()
    serializer_class = PerInfooneSerializer
    pagination_class = SetPaginationInit


class Patinfolist(GenericAPIView):

    queryset = Patient.objects.all()
    pagination_class = SetPaginationInit

    def get(self, request):
        """
        provide patients information list
        :param request:a django rest framework request object
        :return: information list
        """
        # 调用自定义类，创建对象，获取信息列表
        infolist = InfoList()
        pats_list = infolist.get_infolist()

        if len(pats_list) == 0:
            return Response('数据库无数据')

        response = Response(pats_list)

        return response


class GetView(GenericAPIView):
    def get(self, request):
        user_page_size = int(request.GET.get('page_size'))
        # current_page = int(request.GET.get('page'))

        # 得到的是带有数据的查询集对象
        # start_page = (current_page - 1) * user_page_size + 1
        # end_page = current_page * user_page_size
        # patient_list = []
        # for i in range(start_page, end_page + 1):
        #     each_patient = Patient.objects.filter(pid=18+i).values_list()
        #     patient_list.append(each_patient)

        # 得到的是字典格式的数据
        # start_page = (current_page - 1) * user_page_size
        # end_page = current_page * user_page_size
        # 得到查询集对象
        patients = Image.objects.all()[:user_page_size]
        # patients = Image.objects.all()[:3]
        patient_list = []
        for patient in patients:
            # 将查询集对象转换成字典
            each_patient = model_to_dict(patient)
            patient_list.append(each_patient)
        response = HttpResponse(patient_list)
        print len(patients)
        return response


