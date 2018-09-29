import { Injectable } from '@angular/core';

declare var Mock: any;
const Random = Mock.Random;

@Injectable()
export class ForgotPasswordServiceMock {

  constructor() {

    Mock.mock(/\/api\/forgot-password/, 'post', (data) => {
      return true;
    });

    Mock.mock(/\/api\/reset-password\/[A-Za-z0-9-]+\/is-valid/, 'get', (data) => {
      return true;
    });

    Mock.mock(/\/api\/reset-password/, 'post', (data) => {
      return true;
    });


  }
}
