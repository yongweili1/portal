import os

import cv2
import numpy as np
import md.image3d.python.image3d_io as cio

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

        contours = {}
        for index in range(slice_count):
            slice_index = index + 1
            slice = slices[index]
            _, thresh = cv2.threshold(slice, 0, 255, cv2.THRESH_BINARY)
            _, contour, _ = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
            if len(contour) == 0:
                continue

            graphics = []
            for item in contour:
                graphic = []
                for i in item:
                    pt = i[0].tolist()
                    pt.append(index)
                    pt = mask.voxel_to_world(pt)
                    graphic.append(pt.tolist())
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
