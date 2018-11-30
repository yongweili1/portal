import json
import os

from db_context import contour_ctx
from utils.uid_generator import UidGenerator
from config.path_cfg import file_path_ferry


class ContourService(object):
    def __init__(self):
        pass

    def delete(self, image_uid, roi_uid):
        # delete cps files firstly
        contours, msg = contour_ctx.retrieve(image_uid, roi_uid)
        if contours is not None:
            for item in contours:
                if not os.path.isfile(item.cpspath):
                    continue
                os.remove(item.cpspath)
        return contour_ctx.delete(image_uid, roi_uid)

    def create(self, slice_index, roi_uid, contours):
        try:
            success, msg = self.delete(slice_index, roi_uid)
            if not success:
                return success, msg

            for cps in contours:
                contour_uid = UidGenerator.contour_uid()
                file_name = contour_uid
                cpspath = os.path.join(file_path_ferry.cpsPath, file_name)
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
