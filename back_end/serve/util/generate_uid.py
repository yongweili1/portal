import random
import datetime as Time


class GenerateUid(object):
    def __generate_uid(self, type):
        org_root = '1.2.156.112605.'
        type_value = ''
        if type == 'roi':
            type_value = '21.'
        elif type == 'contour':
            type_value = '22.'
        time_stamp = Time.datetime.now().strftime('%Y%m%d.%H%M%S.')
        rdm = str(random.randint(0, 1000))
        _uid = org_root + type_value + time_stamp + rdm
        return _uid

    def RoiUid(self):
        return self.__generate_uid('roi')

    def ContourUid(self):
        return self.__generate_uid('contour')
