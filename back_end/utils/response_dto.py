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
