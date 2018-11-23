# -*- coding: utf-8 -*-
from __future__ import unicode_literals
from md_segmentation3d.impl.vseg_cimpl import autoseg_load_model, autoseg_volume
from md.image3d.python.image3d_io import read_image, write_image


# Create your views here.
from rest_framework.response import Response
from rest_framework.views import APIView
from db_access.models import Series, Roi


class LoadAlg(APIView):
    def get(self, request):
        serid = request.GET.get('seriesuid', None)
        if serid is None:
            return Response('请输入序列UID')

        seriesobject = Series.objects.filter(seriesuid=serid)
        if len(seriesobject) == 0:
            return Response('请输入正确的序列ID')

        volumepath = seriesobject[0].seriespixeldatafilepath

        resp = None  # resp: Load succeed

        return Response(resp)


class GetAlgResult(APIView):

    def get(self, request):

        resp = None
        return Response(resp)

    def post(self, request):
        series_uid = request.data.get('seriesuid', None)
        # roiname = request.data.get('ROIName', None)
        # roicolor = request.data.get('ROIColor', None)

        if series_uid is None:
            return Response('请输入序列UID')

        series_object = Series.objects.filter(seriesuid=series_uid)
        if len(series_object) == 0:
            return Response('请输入正确的序列ID')

        volume_path = series_object[0].seriespixeldatafilepath

        im = read_image(volume_path)
        try:
            model = autoseg_load_model(r'D:\segmentation_model\VSEG_Heart_20180611_01', 0)
            seg = autoseg_volume(im, model)
            write_image(seg, r'D:\segmentation_model\test_result.nii.gz')
        except Exception as ex:
            print ex.message


        # resp = volume_path  # resp: Load succeed
        #
        # return Response(resp)



        # if seriesuid is None or roiname is None or roicolor is None:
        #     return Response('请携带完整的有效参数')
        # try:
        #     seriesobj = Series.objects.get(seriesuid=seriesuid)
        # except Exception as e:
        #     return Response('外键seriesuid无对应的数据对象')
        #
        # if Roi.objects.filter(seriesuid=seriesuid, roiname=roiname):
        #     return Response('ROI命名重复')
        #
        # params = {
        #     'seriesuid': seriesobj,
        #     'roiname': roiname,
        #     'roicolor': roicolor
        # }
        #
        # try:
        #     roiobj = Roi.objects.create(**params)
        #     roiobj.save()
        # except Exception as e:
        #     return Response('ROI 提交失败')

        roi_query = Roi.objects.filter(seriesuid=seriesuid)
        roi_list = []
        for roi in roi_query:
            roi_dict = {}
            roi_dict['ROIId'] = roi.pid
            roi_dict['ROIName'] = roi.roiname
            roi_dict['ROIColor'] = roi.roicolor
            roi_list.append(roi_dict)

        rsp = {
            'code': '200',
            'msg': 'success',
            'data': roi_list,
        }

        return Response(rsp)
