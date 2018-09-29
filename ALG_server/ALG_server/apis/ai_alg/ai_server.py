import sys

import md
import numpy as np
import os
import pandas as pd
from md_ribfracture import load_models, detect_rib_fracture


class AIServer(object):

    def __init__(self):

        self.volume = {}
        pass

    def load_volume(self, volume, series_uid):

        volume1 = np.array(volume)
        im = md.Image3d()
        i.from_numpy(volume1)
        del volume
        del volume1
        self.volume[series_uid] = im
        return True, 'Load succeed!'

    def rib_fracture(self):
        gpu_id = 2
        threshold = 0.8
        model_dir = '/home/lyw/rxl5/boneai/packing_for_xl/models/'
        # output_dir = '/home/lyw/rxl5/pyproject/PKU/webresult/'
        models = load_models(model_dir, gpu_id)
        df_fracture = detect_rib_fracture('web_result', self.volume, models, threshold)
        return float(df_fracture[1, 'x'])
        # df_fracture.to_csv(os.path.join(output_dir, 'results.csv'))
