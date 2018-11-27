from db_context import study_ctx, series_ctx


class SeriesService(object):
    def __init__(self):
        pass

    def get_series_uids(self, patient_id):
        try:
            studylist, msg = study_ctx.retrieve(patient_id)
            if studylist is None:
                return None, msg

            serieslist = []
            for study in studylist:
                series_list, msg = series_ctx.retrieve(study_uid=study.studyuid)
                if series_list is None:
                    return None, msg
                serieslist += series_list

            seriesuidlist = []
            for ser in serieslist:
                seriesuidlist.append(ser.seriesuid)
            return seriesuidlist, None
        except Exception as e:
            return False, e.message

    def get_series_by_uid(self, series_uid):
        try:
            query, msg = series_ctx.retrieve(series_uid=series_uid)
            if query is None or len(query) != 1:
                return False, 'query error'
            else:
                return query[0], None
        except Exception as e:
            return None, e.message

    def upload_volume(self, vol_file_path, series_uid):
        try:
            data = {
                'seriesuid': series_uid,
                'seriespixeldatafilepath': vol_file_path,
                'buildvolumesign': int(0)
            }
            return series_ctx.update(data)
        except Exception as e:
            return False, e.message
