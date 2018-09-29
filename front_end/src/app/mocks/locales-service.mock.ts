import { Injectable } from '@angular/core';

declare var Mock: any;

@Injectable()
export class LocalesServiceMock {
  private locales = [
    {
      id: 'ff1db52662ee48c2b6610785f2d294f5',
      name: 'hello',
      text: '你好',
    },
    {
      id: '3f2bb23f8a404178a9858e3b2bf08f5a',
      name: 'btn.ok',
      text: '确认',
    },
    {
      id: '190e878920854393a16252b626deb76c',
      name: 'btn.cancel',
      text: '取消',
    },
  ];

  constructor() {
    Mock.mock(/\/api\/locales/, 'get', this.locales);
  }
}