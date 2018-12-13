import json
import os

from db_context import contour_ctx, series_ctx, roi_ctx
from utils.segmentation_helper import SegmentationHelper
from utils.uid_generator import UidGenerator
from config.path_cfg import file_path_ferry

from md.image3d.python.image3d_io import read_image, write_image


from md_segmentation3d.impl.vseg_cimpl import autoseg_load_model, autoseg_volume


class ContourService(object):
    def __init__(self):
        pass

    def delete(self, image_uid, roi_uid, contour_type):
        # delete cps files firstly
        contours, msg = contour_ctx.retrieve(image_uid, roi_uid, contour_type)
        if contours is not None:
            for item in contours:
                if not os.path.isfile(item['cpspath']):
                    continue
                os.remove(item['cpspath'])
        return contour_ctx.delete(image_uid, roi_uid)

    def deleteByContourUid(self, contour_uid):
        # delete cps files firstly
        contours, msg = contour_ctx.retrieveByContourUid(contour_uid)
        if contours is not None:
            for item in contours:
                if not os.path.isfile(item['cpspath']):
                    continue
                os.remove(item['cpspath'])
        return contour_ctx.deleteByContourUid(contour_uid)

    def createByContourUid(self, slice_index, roi_uid, contour, contour_type, contour_uid):
        try:
            success, msg = self.deleteByContourUid(contour_uid)
            if not success:
                return success, msg
            cpspath = os.path.join(file_path_ferry.cpsPath, contour_uid)
            with open(cpspath, 'wb') as f:
                f.write(json.dumps(contour[0]))
            params = {
                'roiuid': roi_uid,
                'contouruid': contour_uid,
                'type': contour_type,
                'cpspath': cpspath,
                'imageuid': slice_index
            }
            contour_ctx.create(params)
            return True, None
        except Exception as e:
            return False, e.message

    def create(self, slice_index, roi_uid, contours, contour_type):
        try:
            success, msg = self.delete(slice_index, roi_uid, contour_type)
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
                    'type': contour_type,
                    'cpspath': cpspath,
                    'imageuid': slice_index
                }
                contour_ctx.create(params)
            return True, None
        except Exception as e:
            return False, e.message

    def retrieve(self, image_uid=None, roi_uids=None):
        if roi_uids is None:
            return None, 'roi_uids is None'
        contours = []
        for uid in roi_uids:
            records, msg = contour_ctx.retrieve(image_uid, uid)
            if records is None or len(records) == 0:
                continue
            for record in records:
                if not os.path.isfile(record['cpspath']):
                    continue
                contour = {}
                contour['contouruid'] = record['contouruid']
                contour['imageuid'] = record['imageuid']
                contour['roiuid'] = record['roiuid']
                contour['type'] = record['type']
                with open(record['cpspath'], 'rb') as f:
                    cps = f.read()
                    cps = json.loads(cps)
                    contour['cps'] = cps
                    contours.append(contour)
                    f.close()
        return contours, None

    def get_contours(self, roi_uid):
        if roi_uid is None:
            return None, 'roi_uid is None'
        contours = []

        records, msg = contour_ctx.retrieve(roi_uid=roi_uid)
        if records is None or len(records) == 0:
            return None, 'No contours'
        for record in records:
            if not os.path.isfile(record['cpspath']):
                continue
            contour = {}
            contour['contouruid'] = record['contouruid']
            contour['imageuid'] = record['imageuid']
            contour['roiuid'] = record['roiuid']
            with open(record['cpspath'], 'rb') as f:
                cps = f.read()
                cps = json.loads(cps)
                contour['cps'] = cps
                contours.append(contour)
                f.close()
        return contours, None

    def seg(self, series_uid):
        try:
            series, msg = series_ctx.retrieve(series_uid=series_uid)
            if len(series) == 0:
                return False, 'series_uid is wrong'

            mask_fp = file_path_ferry.volumePath + r'{}_mask.nii.gz'.format(series_uid)

            if os.path.isfile(mask_fp):
                os.remove(mask_fp)

            volume_path = series[0].seriespixeldatafilepath
            im = read_image(volume_path)
            model = autoseg_load_model(r'D:\segmentation_model\VSEG_Heart_20180611_01', 0)
            seg = autoseg_volume(im, model)
            write_image(seg, mask_fp)

            return mask_fp, None
        except Exception as ex:
            return False, ex.message

    def contour_2_mask(self, mask_fp, roi_uid):
        contours, msg = self.get_contours(roi_uid)
        if contours is None:
            return False, 'contours is None'

        contour_dict = {}
        for contour in contours:
            index = contour['imageuid']
            if index in contour_dict:
                contour_dict[index].append(contour['cps'])
            else:
                contour_dict[index] = [contour['cps']]

        SegmentationHelper.contours_to_mask(mask_fp, contour_dict)
        return True, None

    def mask_2_contour(self, roi_uid, mask_fp):
        try:
            if not os.path.isfile(mask_fp):
                return False, 'Please check mask file path.'

            # get current segmentation's contours
            contours = SegmentationHelper.mask_to_contours(mask_fp)
            for index in contours:
                self.create(index, roi_uid, contours[index])
            return True, None
        except Exception as e:
            return False, e.message
