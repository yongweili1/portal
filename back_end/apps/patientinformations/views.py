# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import math

from rest_framework.response import Response
from rest_framework.views import APIView

from serve.util.PAInformation import InfoList


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
