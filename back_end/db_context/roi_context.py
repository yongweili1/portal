from db_context.models import Roi
from db_context.serializer import RoiSerializer


class RoiContext(object):
    def __int__(self):
        pass

    def create(self, data):
        try:
            roi = RoiSerializer(data=data)
            roi.is_valid(raise_exception=True)
            return roi.save(), None
        except Exception as e:
            return False, e.message

    def update(self, data):
        try:
            query = Roi.objects.filter(roiuid=data['roiuid'])
            if len(query) != 1:
                return None

            query[0].roiname = data['roiname']
            query[0].roicolor = data['roicolor']
            query[0].save()
            return True, None
        except Exception as e:
            return False, e.message

    def delete(self, uid):
        try:
            rois = Roi.objects.filter(roiuid=uid)
            if len(rois) != 1:
                return False, 'uid {} not exist.'.format(uid)
            return Roi.objects.filter(roiuid=uid).delete(), None
        except Exception as e:
            return False, e.message

    def retrieve(self, series_uid):
        query = Roi.objects.filter(seriesuid=series_uid)
        rois = []
        for item in query:
            roi = {}
            roi['id'] = item.roiuid
            roi['name'] = item.roiname
            roi['color'] = item.roicolor
            rois.append(roi)
        return rois

    def single(self, uid):
        query = Roi.objects.filter(roiuid=uid)
        if len(query) != 1:
            return None

        roi = {}
        roi['id'] = query[0].roiuid
        roi['name'] = query[0].roiname
        roi['color'] = query[0].roicolor
        roi['seriesuid'] = query[0].seriesuid.seriesuid
        return roi

    def duplicate(self, series_uid, name):
        if Roi.objects.filter(seriesuid=series_uid, roiname=name):
            return True
        else:
            return False
