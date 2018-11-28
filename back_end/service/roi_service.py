from db_context import roi_ctx, contour_ctx


class RoiService(object):
    def __init__(self):
        pass

    def create(self, data):
        duplicate = roi_ctx.duplicate(data['seriesuid'], data['roiname'])
        if duplicate:
            return duplicate, 'duplicated'

        return roi_ctx.create(data)

    def update(self, data):
        try:
            roi = roi_ctx.single(data['roiuid'])
            if not roi:
                return False, 'Can not find roi'

            series_uid = roi['seriesuid']
            duplicate = roi_ctx.duplicate(series_uid, data['roiname'])
            if duplicate:
                return False, 'duplicated'
            data['seriesuid'] = series_uid
            return roi_ctx.update(data)
        except Exception as e:
            return False, e.message

    def delete(self, uids):
        try:
            for uid in uids:
                success, msg = roi_ctx.delete(uid)
                if not success:
                    return success, msg
                # Delete all contours of this roi
                success, msg = contour_ctx.delete(roi_uid=uid)
                if not success:
                    return success, msg
            return True, None
        except Exception as e:
            return False, e.message

    def retrieve(self, series_uid):
        dtos = roi_ctx.retrieve(series_uid)
        return dtos
