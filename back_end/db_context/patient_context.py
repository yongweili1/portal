from db_context.models import Patient


class PatientContext(object):
    def __init__(self):
        pass

    def all(self):
        return Patient.objects.all()

    def delete(self, uids):
        try:
            if uids is None:
                return False, 'uids is None'

            if isinstance(uids, list):
                for uid in uids:
                    Patient.objects.filter(patientid=uid).delete()
            else:
                Patient.objects.filter(patientid=uids).delete()
            return True, None
        except Exception as e:
            return False, e.message
