from db_context.models import Contour

from db_context.serializer import ContourSerializer


class ContourContext(object):
    def __init__(self):
        pass

    def delete(self, image_uid=None, roi_uid=None):
        try:
            query = Contour.objects
            if roi_uid is not None:
                query = query.filter(roiuid=roi_uid)
            if image_uid is not None:
                query = query.filter(imageuid=image_uid)
            query.delete()
            return True, None
        except Exception as ex:
            return False, ex.message

    def create(self, data):
        try:
            contour = ContourSerializer(data=data)
            contour.is_valid(raise_exception=True)
            return contour.save(), None
        except Exception as ex:
            return False, ex.message

    def retrieve(self, image_uid=None, roi_uid=None):
        try:
            query = Contour.objects
            if image_uid is not None:
                query = query.filter(imageuid=image_uid)
            if roi_uid is not None:
                query = query.filter(roiuid=roi_uid)
            if query is not None or len(query) > 0:
                records = []
                for item in query:
                    record = {}
                    record['contouruid'] = item.contouruid
                    record['roiuid'] = item.roiuid
                    record['imageuid'] = item.imageuid
                    record['cpspath'] = item.cpspath
                    records.append(record)
            return records, None
        except Exception as ex:
            return None, ex.message
