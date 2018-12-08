import os

import cv2
import numpy as np
import md.image3d.python.image3d_io as cio
from md.image3d.python.image3d_tools import slice_nn

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
        center_x, center_y, center_z = mask.center()
        size_x, size_y, slice_count = mask.size()
        spacing_x, spacing_y, spacing_z = mask.spacing()
        origin_x, origin_y, origin_z = mask.origin()
        axis_x = mask.axis(0)
        axis_y = mask.axis(1)
        axis_z = mask.axis(2)

        mask.from_numpy(mask.to_numpy(), np.uint8)

        contours = {}
        for index in range(slice_count):
            slice_index = index + 1

            look_at = np.array([center_x, center_y, origin_z + index * spacing_z])
            mask_slice = slice_nn(mask, look_at, axis_x, axis_y, look_at,
                                  [spacing_x, spacing_y], [size_x, size_y], -1024)
            _, thresh = cv2.threshold(mask_slice, 0, 255, cv2.THRESH_BINARY)
            _, contour, _ = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
            if len(contour) == 0:
                continue

            graphics = []
            for item in contour:
                graphic = []
                for i in item:
                    pt = i[0].tolist()
                    pt = [pt[0] + origin_x, pt[1] + origin_y, origin_z + index * spacing_z]
                    graphic.append(pt)
                graphics.append(graphic)

            contours[slice_index] = graphics

        return contours

    @staticmethod
    def contours_to_mask(contours):
        pass


if __name__ == '__main__':
    mask_fp = r'E:\1.2.840.113619.2.55.3.604655767.808.1412648222.944\1.2.840.113619.2.55.3.604655767.808.1412648222.944_mask.nii.gz'
    contours = SegmentationHelper.mask_to_contours(mask_fp)
    pass
