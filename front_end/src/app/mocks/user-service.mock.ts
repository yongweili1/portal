import { Injectable } from '@angular/core';

declare var Mock: any;
let Random = Mock.Random;

@Injectable()
export class UserServiceMock {

  constructor() {
    Mock.mock(/\/api\/user-info/, 'get', () => {
      return {
        'userId': Random.guid(),
        'realName': '用户',
        'telephone': null,
        'positions': '',
        'gender|1': ['0', '1'],
        'email': Random.email(),
        'wechat': null,
        'description': null,
        'userName': 'user',
        'isManage': 0,
        'isSystem': 0,
        'enabled': 1,
        'isLock': 0,
        'lockTime': null,
        'failCount': 0,
        'passwordUpdateTime': new Date(),
        'attr1': null,
        'attr2': null,
        'attr3': null,
        'attr4': null,
        'attr5': null,
        'orgId': null,
        'roles': []
      };
    });
  }
}
