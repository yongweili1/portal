from db_context.models import Study


class StudyContext(object):
    def __init__(self):
        pass

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
