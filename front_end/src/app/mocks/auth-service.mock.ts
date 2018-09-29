import { Injectable } from '@angular/core';

declare var Mock: any;

@Injectable()
export class AuthServiceMock {

  constructor() {
    Mock.mock(/\/api\/auth/, 'post', {
      'token': 'eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJ1c2VyIiwiYXVkaWVuY2UiOiJ3ZWIiLCJjcmVhdGVkIjoxNTAwNDQ0OTA2ODU3LCJleHAiOjE1NjA5MjQ5MDZ9.VE9OEFfkcgwJVnz_hP_yT8ZHK7KVrzcP4ToUDrkiuZjKZx2EWB4VKZi25DmEuHGM1b-vd2-LbeWGh0dxi1l0hg'
    });
  }

}
