from db_context.models import Image
from db_context.serializer import ImageSerializer


class ImageContext(object):
    def __init__(self):
        pass

    def exist(self, imageuid):
        if len(Image.objects.filter(imageuid=imageuid)) != 0:
            return True
        else:
            return False

    def create(self, data):
        try:
            model = ImageSerializer(data=data)
            model.is_valid(raise_exception=True)
            return model.save(), None
        except Exception as ex:
            return False, ex.message

    def update(self, imageuid, data):
        try:
            Image.objects.filter(imageuid=imageuid).update(data)
            return True, None
        except Exception as e:
            return False, e.message

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
