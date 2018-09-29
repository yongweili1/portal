#!/usr/bin/python
# coding:utf-8

import sys
from suds.client import Client


reload(sys)
sys.setdefaultencoding('utf8')


class UploadResult(object):
    def __init__(self):
        # self.customer_wsdl = customer_wsdl
        pass

    def send_result(self, result):
        client = Client('http://www.smit-cloud.com:8005/WebAIService.asmx?WSDL')

        return_message = client.service.Upload(result)

        return return_message
