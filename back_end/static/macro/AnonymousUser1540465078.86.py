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

