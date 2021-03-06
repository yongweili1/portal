# -*-coding:utf-8-*-

from db_context.contour_context import ContourContext
from db_context.image_context import ImageContext
from db_context.macro_context import MacroContext
from db_context.patient_context import PatientContext
from db_context.roi_context import RoiContext
from db_context.series_context import SeriesContext
from db_context.session_context import SessionContext
from db_context.study_context import StudyContext

study_ctx = StudyContext()
series_ctx = SeriesContext()
roi_ctx = RoiContext()
contour_ctx = ContourContext()
macro_ctx = MacroContext()
patient_ctx = PatientContext()
image_ctx = ImageContext()
session_ctx = SessionContext()
