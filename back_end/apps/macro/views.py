#-*-coding:utf-8-*-
import os
import time

from rest_framework.views import APIView

from service import macro_svc, series_svc
from settings import STATIC_ROOT
from utils.macro_recorder import MacroRecorder
from utils.response_dto import ResponseDto


class MacroRecording(APIView):
    def get(self, request):
        """
        Opening and closing macro recording, save script
        :param macro_status: the status of macro recording
        :return:
        """
        macro_status = request.GET.get('macro_status', None)
        user_name = request.user
        if macro_status == 'start':
            MacroRecorder.macro_status = True
        elif macro_status == 'finish':
            macro_name = str(user_name) + str(time.time())
            scriptname = '{}.py'.format(macro_name)
            scriptpath = STATIC_ROOT + '/serve/macro/' + scriptname

            try:
                with open(scriptpath, 'a+') as f:
                    f.write(MacroRecorder.code_header + MacroRecorder.code)
            except:
                MacroRecorder.code = MacroRecorder.code
                return ResponseDto('脚本写入失败')

            MacroRecorder.macro_status = False
            MacroRecorder.code = ''

            data = {
                'scriptname': scriptname,
                'userid': 'anonymous',
                'scriptpath': scriptpath
            }

            success, msg = macro_svc.create(data)
            if not success:
                return ResponseDto(success=success, message=msg)

            return ResponseDto(macro_name)
        else:
            pass

        return ResponseDto('OK')


class RunSript(APIView):
    def get(self, request):
        seriesuid = request.GET.get('seriesuid', '1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373')
        scriptname = request.GET.get('scriptname', 'AnonymousUser1540275438.76')
        if not seriesuid or not scriptname:
            return ResponseDto('请输入有效的参数')

        series, msg = series_svc.get_series_by_uid(seriesuid)
        if not series:
            return ResponseDto(success=False, message=msg)
        volumepath = series.seriespixeldatafilepath
        try:
            command = 'python static/macro/{}.py {} {}'.format(scriptname, seriesuid, volumepath)
            r = os.popen(command)
            info = r.readlines()
            print(info)
        except:
            return ResponseDto('script执行异常')

        return ResponseDto('OK')
