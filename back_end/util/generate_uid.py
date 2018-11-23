import random
import datetime as Time


class GenerateUid(object):
    @classmethod
    def __generate_uid(cls, uid_type):
        org_root = '1.2.156.112605.'
        type_value = ''
        if uid_type == 'roi':
            type_value = '21.'
        elif uid_type == 'contour':
            type_value = '22.'
        time_stamp = Time.datetime.now().strftime('%Y%m%d.%H%M%S.')
        rdm = str(random.randint(0, 1000))
        _uid = org_root + type_value + time_stamp + rdm
        return _uid

    @classmethod
    def roi_uid(cls):
        return cls.__generate_uid('roi')

    @classmethod
    def contour_uid(cls):
        return cls.__generate_uid('contour')
