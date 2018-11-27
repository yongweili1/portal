from db_context.models import Patient
from db_context.serializer import PatientSerializer


class PatientContext(object):
    def __init__(self):
        pass

    def exist(self, patientid):
        if len(Patient.objects.filter(patientid=patientid)) != 0:
            return True
        else:
            return False

    def all(self):
        return Patient.objects.all()

    def create(self, data):
        try:
            contour = PatientSerializer(data=data)
            contour.is_valid(raise_exception=True)
            return contour.save(), None
        except Exception as ex:
            return False, ex.message

    def update(self, patientid, data):
        try:
            Patient.objects.filter(patientid=patientid).update(data)
            return True, None
        except Exception as e:
            return False, e.message

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
