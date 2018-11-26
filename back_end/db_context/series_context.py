from db_context.models import Series


class SeriesContext(object):
    def __init__(self):
        pass

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
