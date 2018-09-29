import { Injectable } from '@angular/core';

declare var Mock: any;
const Random = Mock.Random;

@Injectable()
export class EmailTemplateServiceMock {
  constructor() {
    Mock.mock(/\/api\/email-templates\?/, 'get', {
      'content|15': [{
        'id': '@guid',
        'name': '@name',
        'subject': 'zhuti',
        'createTime': '@date(yyyy-MM-dd HH:mm:ss)',
        'updateTime': '@date(yyyy-MM-dd HH:mm:ss)',
        'content': '',
        'enable': '1'
      }],
      'totalPages': 2,
      'totalElements': 16,
      'number': 0,
      'size': 10
    });

    Mock.mock(/\/api\/email-templates\/[A-Za-z0-9-]+/, 'get', {
      'id': '@guid',
      'name': '@name',
      'subject': 'zhuti',
      'createTime': '@date(yyyy-MM-dd HH:mm:ss)',
      'updateTime': '@date(yyyy-MM-dd HH:mm:ss)',
      'content': '',
      'enable': '1',
    });

    Mock.mock(/\/api\/email-templates\/([\w\d-]+)/, 'delete', (option) => {
      const id = option.url.substr(option.url.lastIndexOf('/') + 1);
      return true;
    });

    Mock.mock(/\/api\/email-templates/, 'post', (option) => {
      return {
        'id': '@guid',
        'name': '@name',
        'subject': 'zhuti',
        'createTime': new Date(),
        'updateTime': new Date(),
        'content': '',
        'enable': '0',
      };
    });

    Mock.mock(/\/api\/email-templates\/[A-Za-z0-9-]+/, 'put', (option) => {
      const id = option.url.substr(option.url.lastIndexOf('/') + 1);
      return {
        'id': id,
        'name': '@name',
        'subject': 'zhuti',
        'createTime': new Date(),
        'updateTime': new Date(),
        'content': '',
        'enable': '0',
      };
    });
  }

}
