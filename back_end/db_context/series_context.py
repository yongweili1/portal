from db_context.models import Series
from db_context.serializer import SeriesSerializer


class SeriesContext(object):
    def __init__(self):
        pass

    def exist(self, seriesuid):
        if len(Series.objects.filter(seriesuid=seriesuid)) != 0:
            return True
        else:
            return False

    def create(self, data):
        try:
            model = SeriesSerializer(data=data)
            model.is_valid(raise_exception=True)
            return model.save(), None
        except Exception as ex:
            return False, ex.message

    def update(self, seriesuid, data):
        try:
            Series.objects.filter(seriesuid=seriesuid).update(data)
            return True, None
        except Exception as e:
            return False, e.message

    def retrieve(self, series_uid=None, study_uid=None):
        try:
            query = Series.objects
            if series_uid is not None:
                query = query.filter(seriesuid=series_uid)
            if study_uid is not None:
                query = query.filter(studyuid=study_uid)
            return query, None
        except Exception as e:
            return None, e.message

    def update(self, data):
        try:
            query = Series.objects.filter(seriesuid=data['seriesuid'])
            query.seriespixeldatafilepath = data['seriespixeldatafilepath']
            query.buildvolumesign = data['buildvolumesign']
            query.save()
            return True, None
        except Exception as e:
            raise False, e.message

    def delete(self, uids):
        try:
            if uids is None:
                return False, 'uids is None'

            if isinstance(uids, list):
                for uid in uids:
                    Series.objects.filter(seriesuid=uid).delete()
            else:
                Series.objects.filter(seriesuid=uids).delete()
            return True, None
        except Exception as e:
            return False, e.message
