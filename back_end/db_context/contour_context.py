from db_context.models import Contour

from db_context.serializer import ContourSerializer


class ContourContext(object):
    def __init__(self):
        pass

    def delete(self, image_uid=None, roi_uid=None, type=None):
        try:
            query = Contour.objects
            if type is not None:
                query = query.filter(type=type)
            if roi_uid is not None:
                query = query.filter(roiuid=roi_uid)
            if image_uid is not None:
                query = query.filter(imageuid=image_uid)
            query.delete()
            return True, None
        except Exception as ex:
            return False, ex.message

    def delete_by_contouruid(self, contour_uid=None):
        try:
            query = Contour.objects
            if contour_uid is not None:
                query = query.filter(contouruid=contour_uid)
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

    def retrieve(self, image_uid=None, roi_uid=None, contour_type=None):
        try:
            query = Contour.objects
            if contour_type is not None:
                query = query.filter(type=contour_type)
            if image_uid is not None:
                query = query.filter(imageuid=image_uid)
            if roi_uid is not None:
                query = query.filter(roiuid=roi_uid)
            if query is not None or len(query) > 0:
                records = []
                for item in query:
                    record = {}
                    record['contouruid'] = item.contouruid
                    record['roiuid'] = item.roiuid_id
                    record['imageuid'] = item.imageuid
                    record['cpspath'] = item.cpspath
                    record['type'] = item.type
                    records.append(record)
            return records, None
        except Exception as ex:
            return None, ex.message

    def retrieve_by_contouruid(self, contour_uid=None):
        try:
            query = Contour.objects
            if contour_uid is not None:
                query = query.filter(contouruid=contour_uid)
            if query is not None and len(query) == 1:
                record = {}
                item = query[0]
                record['contouruid'] = item.contouruid
                record['roiuid'] = item.roiuid_id
                record['imageuid'] = item.imageuid
                record['cpspath'] = item.cpspath
                record['type'] = item.type
            return record, None
        except Exception as ex:
            return None, ex.message
