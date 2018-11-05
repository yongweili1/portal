# -*-coding:utf-8-*-
import os
import channels.asgi

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "back_end.settings")
channel_layer = channels.asgi.get_channel_layer()
