# -*- coding: utf-8 -*-
from __future__ import unicode_literals

import urllib
import urllib2

# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView

from patientinformations.models import Series
from md.image3d.python.image3d_io import read_image
# from back_end.util.alg_server_API import LOAD_ALG_URL, GET_ALGRESULT_URL
import ConfigParser


# conf = ConfigParser.ConfigParser()
# conf.read('back_end/util/serverApi.ini')
# load_alg_url = conf.get("algorithmApi", "load_alg_url")
# get_alg_result_url = conf.get("algorithmApi", "get_alg_result_url")


class LoadAlg(APIView):
    def get(self, request):
        serid = request.GET.get('seriesuid', None)
        if serid is None:
            return Response('请输入序列UID')

        seriesobject = Series.objects.filter(seriesuid=serid)
        if len(seriesobject) == 0:
            return Response('请输入正确的序列ID')

        volumepath = seriesobject[0].seriespixeldatafilepath

        # vol = read_image(volumepath)
        # vol = vol.to_numpy()
        vol = read_image(volumepath).to_numpy().tolist()

        # with open(volumepath, 'rb') as f:
        #     vol = f.read()
        # print(type(vol))
        # print(sys.getsizeof(vol))

        data = {
            'seriesuid': serid,
            'volume': vol
        }
        data = urllib.urlencode(data)
        req = urllib2.Request(load_alg_url, data)
        try:
            response = urllib2.urlopen(req)
        except Exception as e:
            return Response('algproxy_server Error')
        resp = response.read()  # resp: Load succeed

        return Response(resp)


class GetAlgResult(APIView):

    def get(self, request):

        req = urllib2.Request(get_alg_result_url)
        try:
            response = urllib2.urlopen(req)
        except Exception as e:
            return Response('algproxy_server Error')
        resp = response.read()
        print(resp)

        return Response(resp)
