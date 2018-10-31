from PIL import Image, ImageDraw
import pydicom
from StringIO import StringIO
import base64
import numpy as np
from md.image3d.python.image3d_vis import slice_to_bytes, bytes_to_colors
import ConfigParser
import md.image3d.python.image3d_io as cio
import os

Config = ConfigParser.ConfigParser()
Config.read('config.ini')


class ImgCutter(object):
    def __init__(self, slice_info, image3d, img_folder):
        self.img_folder = img_folder

        self.data = []
        img_array = image3d.to_numpy()

        min_box_voxel = slice_info[0]['min_voxel']
        max_box_voxel = slice_info[0]['max_voxel']

        for i in range(int(min_box_voxel[2]), int(max_box_voxel[2])+1):

            pixels = img_array[i, :, :]
            pixels = slice_to_bytes(pixels, -450, 1050)  # CT Bone
            pixels = bytes_to_colors(pixels, 0, self.__get_colormap())

            for item in slice_info:
                if item['voxel_slice_num'] == i:
                    image_uid = item['image_uid']

                    self.data.append({
                        'image_uid': image_uid,
                        'pixel_spacing': image3d.spacing(),
                        'pixel_data': pixels,
                        'min_box_position': min_box_voxel,
                        'max_box_position': max_box_voxel
                        })

    def __get_colormap(self):
        colormap = np.zeros((256 * 3,), dtype=np.int32)
        for i in range(256):
            colormap[i * 3] = 255
            colormap[i * 3 + 1] = 0
            colormap[i * 3 + 2] = 0
        return colormap

    def handle(self):
        base64img_dic = {}
        if len(self.data) == 0:
            return base64img_dic

        for item in self.data:
            im = Image.fromarray(item['pixel_data'])

            draw_im = ImageDraw.Draw(im)

            x1 = item['min_box_position'][0]
            y1 = item['min_box_position'][1]
            x2 = item['max_box_position'][0]
            y2 = item['max_box_position'][1]
            x1 = x1
            y1 = y1
            x2 = x2
            y2 = y2

            draw_im.line([x1, y1, x1, y2], fill=(255, 100, 0), width=1)
            draw_im.line([x1, y2, x2, y2], fill=(255, 100, 0), width=1)
            draw_im.line([x2, y2, x2, y1], fill=(255, 100, 0), width=1)
            draw_im.line([x2, y1, x1, y1], fill=(255, 100, 0), width=1)

            text_coord = [x1 + 1, y1 - 10]
            draw_im.text(text_coord, " ", fill=(255, 100, 0))
            del draw_im

            x1 = x1 - Config.getint("db", 'extend_pixel')
            y1 = y1 - Config.getint("db", 'extend_pixel')
            x2 = x2 + Config.getint("db", 'extend_pixel')
            y2 = y2 + Config.getint("db", 'extend_pixel')

            x1 = x1
            y1 = y1
            x2 = x2
            y2 = y2

            if Config.getint("db", 'crop_image'):
                im = im.crop((x1, y1, x2, y2))
            if Config.getint("db", 'save_image'):
                img_file = os.path.join(self.img_folder, item['image_uid']) + '.jpg'
                im.save(img_file)
            output_buffer = StringIO()
            im.save(output_buffer, 'JPEG')
            binary_data = output_buffer.getvalue()
            base64_data = base64.b64encode(binary_data)
            base64img_dic[item['image_uid']] = base64_data

        return base64img_dic


if __name__ == '__main__':
    slice_info = [{'instance_num': 634,
                         'best_instance_num': 634,
                         'voxel_slice_num':75,
                         'image_uid': '1.2.840.113704.1.111.1888.1508053946.10640',
                         'rectangle_patient': (12,25,34,65),
                         'rectangle_pixel': (15,24,65,87),
                         'probability': 1,
                         'rib_class': 1,
                         'rib_location': 1,
                         'min_voxel': (1, 2, 3),
                         'max_voxel': (4, 5, 6)}]
    images, tag = cio.read_dicom_series(r'/home/lyw/rxl5/1.2.840.113704.1.111.12164.1508053864.7/')
    cutter = ImgCutter(slice_info, images)
    a = cutter.handle()

