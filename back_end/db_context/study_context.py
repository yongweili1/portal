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