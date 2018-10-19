# -*- coding: utf-8 -*-


import os, django

import sys

import thread

from twisted_client import be_factory

from twisted.internet import reactor

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "back_end.settings")

django.setup()

sys.path.insert(0, r'E:\research_ly\portal\back_end\apps')

reactor.connectTCP('127.0.0.1', 8001, be_factory)

thread.start_new_thread(reactor.run, (0,))

a = sys.argv[0]

from apps.connect_image.view_model import load_volume

result = load_volume(serid='1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373', user_ip='127.0.0.1',
                     command='load', server_name='image',
                     volumepath=r'D:\volume\SHEN YU-Thorax^10_ZRY_LDCT_Head_first (Adult)-CT.nii.gz', )
print(result.kwargs)

from apps.connect_image.view_model import get_image

result = get_image(server_name='image', seriesuid='1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373',
                   height='400', user_ip='127.0.0.1', width='400', command='show', display_view='all', )
print(result.kwargs)
