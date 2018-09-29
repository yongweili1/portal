"""
WSGI config for image_server project.

It exposes the WSGI callable as a module-level variable named ``application``.

For more information on this file, see
https://docs.djangoproject.com/en/1.11/howto/deployment/wsgi/
"""

import os
from wsgiref.simple_server import make_server

import django
from django.core.handlers.wsgi import WSGIHandler

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "image_server.settings")


class RequestManager(object):
    """
    Request manager
    """
    def __init__(self):

        # key: session value: process
        self.processes = {}
        self.port = 8888

    def __call__(self, environ):
        self.handle_request(environ)

    def handle_request(self, environ):
        remote_ip = environ['REMOTE_ADDR']
        print(remote_ip)
        # if remote_ip not in self.processes:
        #     self.port += 1
        #     httpd = make_server('', self.port, application)
        #     httpd.serve_forever()
        #
        #     self.processes[remote_ip] = None


request_manager = RequestManager()


class MyWSGIHandler(WSGIHandler):
    def __call__(self, environ, start_response):
        request_manager(environ)
        return super(MyWSGIHandler, self).__call__(environ, start_response)


def get_mywsgi_application():
    django.setup(set_prefix=False)
    return MyWSGIHandler()


application = get_mywsgi_application()



