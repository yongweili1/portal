import os

import cv2
import numpy as np
import md.image3d.python.image3d_io as cio
from scipy.ndimage import binary_fill_holes
from skimage.draw import line_aa

from netbase.c_log import log


class SegmentationHelper(object):

    @staticmethod
    def mask_to_contours(mask_fp):
        """
        Convert mask to contours
        :param mask_fp: Mask file path
        :return: Array of contours
        """
        log.dev_info('Mask file path: {}'.format(mask_fp))
        if not os.path.isfile(mask_fp):
            log.dev_error('Path {} is not a mask file')
            return None
        mask = cio.read_image(mask_fp)
        mask.from_numpy(mask.to_numpy(), np.uint8)
        size_x, size_y, slice_count = mask.size()
        slices = mask.to_numpy()

        rst = {}
        for index in range(slice_count):
            slice = slices[index]
            _, thresh = cv2.threshold(slice, 0, 255, cv2.THRESH_BINARY)
            _, contours, _ = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
            if len(contours) == 0:
                continue

            graphics = []
            for contour in contours:
                graphic = []
                for cp in contour:
                    pt = cp[0].tolist()
                    pt.append(index + 1)
                    graphic.append(pt)
                graphics.append(graphic)

            rst[index + 1] = graphics

        return rst

    @staticmethod
    def contours_to_mask(mask_fp, contours):
        """
        Convert contour to mask(.nii.gz)
        :param contours: dictionary of contours
        :param size: mask shape
        :return: mask file path
        """
        mask = cio.read_image(mask_fp)
        size_x, size_y, slice_count = mask.size()
        mask.from_numpy(np.zeros(np.array([slice_count, size_y, size_x])), np.uint8)
        slices = mask.to_numpy()
        for index in contours:
            slice = slices[int(index) - 1]
            graphics = contours[index]
            for pts in graphics:
                num_pts = len(pts)
                for i in range(num_pts):
                    pt = pts[i]
                    prev = pts[num_pts - 1] if i == 0 else pts[i - 1]
                    rr, cc, _ = line_aa(prev[1], prev[0], pt[1], pt[0])
                    rr[rr >= size_y] = size_y - 1
                    cc[cc >= size_x] = size_x - 1
                    slice[rr, cc] = 1

            slices[int(index) - 1] = binary_fill_holes(slice).astype(np.uint8)
        mask.from_numpy(slices, np.uint8)
        cio.write_image(mask, mask_fp)


if __name__ == '__main__':
    # mask_fp = r'E:\1.2.840.113619.2.55.3.604655767.808.1412648222.944\1.2.840.113619.2.55.3.604655767.808.1412648222.944_mask.nii.gz'
    # contours = SegmentationHelper.mask_to_contours(mask_fp)
    pass

    # series_uid = '1.2.840.113619.2.55.3.604655767.808.1412648222.944'
    # size = [512, 512, 77]
    # roi_uid = '1.2.156.112605.21.20181210.140242.96'
    # contours, msg = contour_svc.get_contours(roi_uid)
    # SegmentationHelper.contours_to_mask(series_uid, contours, size)
    # pass
