from db_context.models import Image


class ImageContext(object):
    def __init__(self):
        pass

    def retrieve(self, series_uid=None):
        try:
            query = Image.objects
            if series_uid is not None:
                query = query.filter(seriesuid=series_uid)
            return query, None
        except Exception as e:
            return None, e.message

    def count(self, series_uid=None):
        images, msg = self.retrieve(series_uid)
        if images:
            return images.count()
        else:
            return 0
