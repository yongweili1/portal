import os

from django.http import StreamingHttpResponse
from rest_framework.response import Response


class ResponseDto(Response):
    def __init__(self, data=None, success=True, message=None,
                 status=None, template_name=None, headers=None,
                 exception=False, content_type=None):
        res = {
            'success': success,
            'message': message,
            'data': data
        }
        super(ResponseDto, self).__init__(data=res, status=status,
                                          template_name=template_name, headers=headers,
                                          exception=exception, content_type=content_type)


class FileResponseDto(StreamingHttpResponse):
    def __init__(self, fp, *args, **kwargs):
        if not os.path.isfile(fp):
            super(FileResponseDto, self)\
                .__init__(streaming_content=(), status=404, reason='File path error')
            return

        def file_iterator(fp, chunk_size=1024):
            with open(fp, 'rb') as f:
                while True:
                    block = f.read(chunk_size)
                    if block:
                        yield block
                    else:
                        break

        super(FileResponseDto, self).__init__(streaming_content=file_iterator(fp), *args, **kwargs)
        self['Content-Type'] = 'application/octet-stream'
        file_name = fp.split('\\')[-1].split('/')[-1]
        self['Content-Disposition'] = 'attachment;filename="{0}"'.format(file_name)
