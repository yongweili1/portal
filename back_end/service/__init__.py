from service.contour_service import ContourService
from service.macro_service import MacroService
from service.pacs_service import PacsService
from service.patient_service import PatientService
from service.roi_service import RoiService
from service.series_service import SeriesService
from service.session_service import SessionService

series_svc = SeriesService()
contour_svc = ContourService()
roi_svc = RoiService()
macro_svc = MacroService()
pacs_svc = PacsService()
patient_svc = PatientService()
session_svc = SessionService()
