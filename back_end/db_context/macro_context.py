# -*-coding:utf-8-*-
from db_context.serializer import ScriptSerializer


class MacroContext(object):
    def create(self, data):
        try:
            macro = ScriptSerializer(data=data)
            macro.is_valid(raise_exception=True)
            return macro.save(), None
        except Exception as ex:
            return False, ex.message
