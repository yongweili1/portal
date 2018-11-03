from PIL import Image, ImageDraw
import pydicom
from StringIO import StringIO
import base64
import numpy as np
from md.image3d.python.image3d_vis import slice_to_bytes, bytes_to_colors
import ConfigParser
import md.image3d.python.image3d_io as cio

Config = ConfigParser.ConfigParser()
Config.read('config.ini')


class ImgCutter(object):
    def __init__(self, slice_info, images):
        self.data = []
        if slice_info is not None and len(slice_info) > 0:
            for item in slice_info:
                ds = self.__get_dataset(images, item['image_uid'])

                pixels = np.frombuffer(ds.PixelData, dtype=self.__get_data_type(ds))
                pixels = np.reshape(pixels, (ds.Rows, ds.Columns))
                pixels = slice_to_bytes(pixels, 300, 1500)  # CT Bone
                pixels = bytes_to_colors(pixels, 0, self.__get_colormap())

                self.data.append({
                    'image_uid': item['image_uid'],
                    'image_position_patient': ds.ImagePositionPatient,
                    'pixel_spacing': ds.PixelSpacing,
                    'size': (ds.Columns, ds.Rows),
                    'pixel_data': pixels,
                    'box_position': item['rectangle_patient']
                })

    def __get_colormap(self):
        colormap = np.zeros((256 * 3,), dtype=np.int32)
        for i in range(256):
            colormap[i * 3] = 255
            colormap[i * 3 + 1] = 0
            colormap[i * 3 + 2] = 0
        return colormap

    def __get_dataset(self, images, img_uid):
        if images is None:
            return None
        for image in images:
            if image.SOPInstanceUID == img_uid:
                return image
        return None

    def __get_data_type(self, dataset):
        bits_allocated = dataset.BitsAllocated
        pixel_representation = dataset.PixelRepresentation
        datatype = np.uint8
        if bits_allocated == 8 and pixel_representation == 0:
            datatype = np.uint8
        elif bits_allocated == 8 and pixel_representation == 1:
            datatype = np.int8
        elif bits_allocated == 16 and pixel_representation == 0:
            datatype = np.uint16
        elif bits_allocated == 16 and pixel_representation == 1:
            datatype = np.int16
        elif bits_allocated == 32:
            datatype = np.complex64
        elif bits_allocated == 64:
            datatype = np.complex128
        return datatype

    def handle(self):
        base64img_dic = {}
        if len(self.data) == 0:
            return base64img_dic

        for item in self.data:
            im = Image.fromarray(item['pixel_data'])
            image_position_patient = item['image_position_patient']
            patient_position_x = float(image_position_patient[0])
            patient_position_y = float(image_position_patient[1])
            box_position = item['box_position']

            draw_im = ImageDraw.Draw(im)

            x1 = (box_position[0] - patient_position_x)/item['pixel_spacing'][0]
            y1 = (box_position[1] - patient_position_y)/item['pixel_spacing'][1]
            x2 = (box_position[2] - patient_position_x)/item['pixel_spacing'][0]
            y2 = (box_position[3] - patient_position_y)/item['pixel_spacing'][1]
            x1 = x1 if x1 > 0 else 0
            y1 = y1 if y1 > 0 else 0
            x2 = x2 if x2 < item['size'][0] * item['pixel_spacing'][0] else item['size'][0] * item['pixel_spacing'][0]
            y2 = y2 if y2 < item['size'][1] * item['pixel_spacing'][1] else item['size'][1] * item['pixel_spacing'][1]

            draw_im.line([x1, y1, x1, y2], fill=(255, 100, 0), width=1)
            draw_im.line([x1, y2, x2, y2], fill=(255, 100, 0), width=1)
            draw_im.line([x2, y2, x2, y1], fill=(255, 100, 0), width=1)
            draw_im.line([x2, y1, x1, y1], fill=(255, 100, 0), width=1)

            text_coord = [x1 + 1, y1 - 10]
            draw_im.text(text_coord, "100%", fill=(255, 100, 0))
            del draw_im

            x1 = x1 - Config.getint("db", 'extend_pixel')
            y1 = y1 - Config.getint("db", 'extend_pixel')
            x2 = x2 + Config.getint("db", 'extend_pixel')
            y2 = y2 + Config.getint("db", 'extend_pixel')

            x1 = x1 if x1 > 0 else 0
            y1 = y1 if y1 > 0 else 0
            x2 = x2 if x2 < item['size'][0] * item['pixel_spacing'][0] else item['size'][0] * item['pixel_spacing'][0]
            y2 = y2 if y2 < item['size'][1] * item['pixel_spacing'][1] else item['size'][1] * item['pixel_spacing'][1]

            if Config.getint("db", 'crop_image'):
                im = im.crop((x1, y1, x2, y2))
            if Config.getint("db", 'save_image'):
                im.save(r'/soft/pyproject/PKU/webresult/img/{}.jpg'.format(item['image_uid']))
            output_buffer = StringIO()
            im.save(output_buffer, 'JPEG')
            binary_data = output_buffer.getvalue()
            base64_data = base64.b64encode(binary_data)
            base64img_dic[item['image_uid']] = base64_data

        return base64img_dic


if __name__ == '__main__':
    slice_info = [[634,'1.2.840.113704.1.111.1888.1508053946.10640', [(22,23),(27,23),(22,28),(72,103)], 1, '','']]
    images = [pydicom.read_file(r'E:\data\1.2.840.113704.1.111.12164.1508053864.7\1.2.840.113704.1.111.1888.1508053946.10640.dcm')]
    cutter = ImgCutter(slice_info, images)
    cutter.handle()

