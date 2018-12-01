#!/usr/bin/env.python
# -*-coding:utf-8-*-
import unittest
import os


def suite():
    suite = unittest.defaultTestLoader.discover(
        start_dir=os.path.dirname(__file__),
        pattern='test_*.py',
        top_level_dir=None)
    return suite


if __name__ == '__main__':
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "back_end.settings")
    unittest.TextTestRunner(verbosity=2).run(suite())
