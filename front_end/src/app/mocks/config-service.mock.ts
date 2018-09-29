import { Injectable } from '@angular/core';

declare var Mock: any;

@Injectable()
export class ConfigServiceMock {

  private conf = [
    {
      confId: '1a2a2ca1567c4f4d9583de2a61a81050',
      confName: 'sys.version',
      confValue: '20170801-121',
      description: '系统版本'
    },
  ];

  constructor() {
    Mock.mock(/\/api\/config/, 'get', this.conf);
  }

}
