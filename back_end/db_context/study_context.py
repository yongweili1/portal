from db_context.models import Study
from db_context.serializer import StudySerializer


class StudyContext(object):
    def __init__(self):
        pass

    def exist(self, studyuid):
        if len(Study.objects.filter(studyuid=studyuid)) != 0:
            return True
        else:
            return False

    def create(self, data):
        try:
            contour = StudySerializer(data=data)
            contour.is_valid(raise_exception=True)
            return contour.save(), None
        except Exception as ex:
            return False, ex.message

    def update(self, studyuid, data):
        try:
            Study.objects.filter(studyuid=studyuid).update(data)
            return True, None
        except Exception as e:
            return False, e.message

    def retrieve(self, patient_id):
        try:
            studys = Study.objects.filter(patientid=patient_id)
            return studys, None
        except Exception as e:
            return None, e.message

    def delete(self, uids):
        try:
            if uids is None:
                return False, 'uids is None'

            if isinstance(uids, list):
                for uid in uids:
                    Study.objects.filter(studyuid=uid).delete()
            else:
                Study.objects.filter(studyuid=uids).delete()
            return True, None
        except Exception as e:
            return False, e.message
