import datetime as Time
import random


class UidGenerator(object):
    @classmethod
    def __generate_uid(cls, type_value):
        org_root = '1.2.156.112605.'
        time_stamp = Time.datetime.now().strftime('%Y%m%d.%H%M%S.')
        rdm = str(random.randint(0, 1000))
        _uid = org_root + type_value + time_stamp + rdm
        return _uid

    @classmethod
    def roi_uid(cls):
        return cls.__generate_uid('21.')

    @classmethod
    def contour_uid(cls):
        return cls.__generate_uid('22.')
