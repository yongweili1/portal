# -*- coding: utf-8 -*-
"""
WSGI config for mytestone project.

It exposes the WSGI callable as a module-level variable named ``application``.

For more information on this file, see
https://docs.djangoproject.com/en/1.11/howto/deployment/wsgi/
"""

import os
import sys
sys.path.append('..')
from netbase.c_log import log

from django.core.wsgi import get_wsgi_application

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "back_end.settings")

log.create_log()
log.dev_info('enter web_be with uwsgi, print first log...')

application = get_wsgi_application()
