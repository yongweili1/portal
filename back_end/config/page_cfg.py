from rest_framework.pagination import PageNumberPagination


class SetPaginationInit(PageNumberPagination):
    # use "page_size" and "page" to modify default setting
    page_size = 5
    page_size_query_param = page_size
    # page_query_param = 'page'
    max_page_size = 5
