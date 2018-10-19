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

seriesuid = sys.argv[1]
volumepath = sys.argv[2]

from apps.connect_image.view_model import load_volume
result = load_volume(user_ip='127.0.0.1',volumepath=r'{}'.format(volumepath),command='load',seriesuid=seriesuid, server_name='image',)
print(result.kwargs)
from apps.connect_image.view_model import get_image
result = get_image(server_name='image',seriesuid=seriesuid, height='400',user_ip='127.0.0.1',width='400',command='show',display_view='all',)
print(result.kwargs)
