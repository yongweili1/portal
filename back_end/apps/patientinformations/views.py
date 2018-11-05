# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import math

from django.http import HttpResponse
from rest_framework.response import Response
from rest_framework.views import APIView

from serve.util.PAInformation import InfoList
from dwebsocket import require_websocket, accept_websocket


class Patinfolist(APIView):

    def get(self, request):
        """
        provide patients information list
        :param size:the number of data on the current page
        :param page:current page
        :return: information list
        """
        size = int(request.GET.get('size', 15))
        page = int(request.GET.get('page', 0))

        # 调用自定义类，创建对象，获取信息列表
        infolist = InfoList()
        pats_list = infolist.get_infolist()
        totalelements = len(pats_list)

        if totalelements == 0:
            return Response('数据库无数据')

        totalpages = int(math.ceil(float(totalelements)/float(size)))
        totalpatients = pats_list[size*page:size*(page+1)]
        numberofelements = len(totalpatients)

        data = {
            'content': totalpatients,
            'totalPages': totalpages,
            'totalElements': totalelements,
            'size': size,
            'number': page,
            'numberOfElements': numberofelements
        }

        return Response(data)


@require_websocket
def websocket(request):
    if not request.is_websocket():
        try:
            message = request.GET['message']
            return HttpResponse(message)
        except Exception as e:
            return HttpResponse('fail')
    else:
        Websocket = request.websocket
        for message in request.websocket:
            print(message)
            Websocket.send(message)


# class GetView(APIView):
#     def get(self, request):
#         user_page_size = int(request.GET.get('page_size'))
#         # current_page = int(request.GET.get('page'))
#
#         # 得到的是带有数据的查询集对象
#         # start_page = (current_page - 1) * user_page_size + 1
#         # end_page = current_page * user_page_size
#         # patient_list = []
#         # for i in range(start_page, end_page + 1):
#         #     each_patient = Patient.objects.filter(pid=18+i).values_list()
#         #     patient_list.append(each_patient)
#
#         # 得到的是字典格式的数据
#         # start_page = (current_page - 1) * user_page_size
#         # end_page = current_page * user_page_size
#         # 得到查询集对象
#         patients = Image.objects.all()[:user_page_size]
#         # patients = Image.objects.all()[:3]
#         patient_list = []
#         for patient in patients:
#             # 将查询集对象转换成字典
#             each_patient = model_to_dict(patient)
#             patient_list.append(each_patient)
#         response = HttpResponse(patient_list)
#         print len(patients)
#         return response
