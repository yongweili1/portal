import json
import os

from db_context import contour_ctx
from utils.uid_generator import UidGenerator


class ContourService(object):
    def __init__(self):
        pass

    def delete(self, image_uid, roi_uid):
        return contour_ctx.delete(image_uid, roi_uid)

    def create(self, slice_index, roi_uid, contours):
        try:
            success, msg = contour_ctx.delete(slice_index, roi_uid)
            if not success:
                return success, msg

            for cps in contours:
                contour_uid = UidGenerator.contour_uid()
                file_name = contour_uid
                cpspath = os.path.join(self.filepath, file_name)
                with open(cpspath, 'wb') as f:
                    f.write(json.dumps(cps))

                params = {
                    'roiuid': roi_uid,
                    'contouruid': contour_uid,
                    'cpspath': cpspath,
                    'imageuid': slice_index
                }
                contour_ctx.create(params)
            return True, None
        except Exception as e:
            return False, e.message

    def retrieve(self, image_uid, roi_uid=None):
        return contour_ctx.retrieve(image_uid, roi_uid)
