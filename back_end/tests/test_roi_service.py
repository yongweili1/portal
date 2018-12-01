import unittest

from service import roi_svc


class RoiServiceTest(unittest.TestCase):
    def tearDown(self):
        pass

    def setUp(self):
        pass

    @classmethod
    def tearDownClass(cls):
        pass

    @classmethod
    def setUpClass(cls):
        pass

    def test_create(self):
        pass

    def test_delete(self):
        pass

    def test_retrieve(self):
        series_uid = ''
        rois = roi_svc.retrieve(series_uid=series_uid)
        pass


if __name__ == '__main__':
    unittest.main()
