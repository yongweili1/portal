from back_end.macro.macro import test

test.add(4, 2)
test.reduce(4, 2)
test.mul(4, 2)
test.divis(4, 2)

# -*- coding: utf-8 -*-
import os,django
os.environ.setdefault("DJANGO_SETTINGS_MODULE", "back_end.settings")# project_name 项目名称
django.setup()

import sys
sys.path.insert(0, r'E:\research_ly\portal\back_end\apps\connect_image')
import views