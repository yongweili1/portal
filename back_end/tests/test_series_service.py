import unittest


class SeriesServiceTest(unittest.TestCase):
    def tearDown(self):
        print('tearDown')

    def setUp(self):
        print('setUp')

    @classmethod
    def tearDownClass(cls):
        print('tearDownClass')

    @classmethod
    def setUpClass(cls):
        print('setUpClass')

    def test_create(self):
        print('test_create')

    def test_delete(self):
        print('test_delete')


# https://www.cnblogs.com/feng0815/p/8045850.html
if __name__ == '__main__':
    unittest.main()