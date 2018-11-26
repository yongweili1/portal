from db_context.roi_context import RoiContext

roi_context = RoiContext()


class RoiService(object):
    def __init__(self):
        pass

    def create(self, data):
        duplicate = roi_context.duplicate(data['seriesuid'], data['roiname'])
        if duplicate:
            return duplicate, 'duplicated'

        return roi_context.create(data)

    def update(self, data):
        try:
            roi = roi_context.single(data['roiuid'])
            if not roi:
                return False, 'Can not find roi'

            series_uid = roi['seriesuid']
            duplicate = roi_context.duplicate(series_uid, data['roiname'])
            if duplicate:
                return False, 'duplicated'
            data['seriesuid'] = series_uid
            return roi_context.update(data)
        except Exception as e:
            return False, e.message

    def delete(self, uids):
        try:
            for uid in uids:
                success, msg = roi_context.delete(uid)
                if not success:
                    return success, msg
            return True, None
        except Exception as e:
            return False, e.message

    def retrieve(self, series_uid):
        dtos = roi_context.retrieve(series_uid)
        return dtos
