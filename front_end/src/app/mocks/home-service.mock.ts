import { Injectable } from '@angular/core';

import * as _ from 'lodash';
import * as moment from 'moment';
import { getQueryStringByName } from '../core/helpers';

declare var Mock: any;

const Random = Mock.Random;
@Injectable()
export class HomeServiceMock {

  public barData = [];

  public getArrayRandom(arr, num: number = 1) {
    return arr
      && arr.length
      && arr[Math.floor(Math.random() * arr.length)];
  }

  public randomValue() {
    const temp = this.getArrayRandom([1540, 7500, 2300, 4700, 1527, 6501, 9497, 6503]);
    return temp;
  }

  public createhomeByDay(i: number) {
    return Mock.mock({
      'date': moment(new Date()).subtract(i, 'day').format('YYYY-MM-DD'),
      'dateValue': this.randomValue(),
    });
  }
  public createSaleBar(option) {
    this.barData = [];
    for (let i = 9; i >= 0; i--) {
      this.barData.push(this.createhomeByDay(i));
    }
    return this.barData;
  }

  constructor() {
    Mock.mock(/\/api\/home\/total-sales/, 'get', {
      UGS: 1024345,
      transactions: 12300,
      dailyUGS: 10023,
    });
    Mock.mock(/\/api\/home\/today-sales/, 'get', {
      UGS: 10202,
      transactions: 120,
      dayOnDay: 3,
    });
    Mock.mock(/\/api\/home\/total-arrear/, 'get', {
      UGS: 4345,
      ratio: -7,
      UGSperCapita: 23,
    });
    Mock.mock(/\/api\/home\/total-refund/, 'get', {
      UGS: 1259,
      transactions: 12,
    });

    // 饼图
    Mock.mock(/\/api\/home\/opera-pie/, 'get', {
      normal: 40800,
      balanceAlarm: 12000,
      arrearage: 7200,
    });
    // 柱状图
    Mock.mock(/\/api\/home\/sales-bar/, 'get', (option) => {
      return this.createSaleBar(option);
    });
    // sales Ranking
    Mock.mock(/\/api\/home\/sales-rank/, 'get', [
      {
        saleName: 'Agency -1',
        UGS: 3320,
      },
      {
        saleName: 'Direct Hall-1',
        UGS: 2700,
      },
      {
        saleName: 'Agency -2',
        UGS: 2200,
      },
      {
        saleName: 'Agency -3',
        UGS: 1800,
      },
      {
        saleName: 'Agency -4',
        UGS: 1550,
      }
    ]);
    // opera right
    Mock.mock(/\/api\/home\/opera-right/, 'get', [
      {
        saleName: 'Agency -2',
        timeAgo: 1,
        sold: 50,
      },
      {
        saleName: 'Agency -1',
        timeAgo: 2,
        sold: 80,
      },
      {
        saleName: 'Direct Hall-1',
        timeAgo: 3,
        sold: 40,
      },
      {
        saleName: 'Direct Hall-1',
        timeAgo: 4,
        sold: 64,
      },
      {
        saleName: 'Direct Hall-1',
        timeAgo: 5,
        sold: 56,
      }
    ]);

    Mock.mock(/\/api\/dashboard\/components/, 'get', [
      {
        'id': '1',
        'name': 'total-sales',
        'title': null,
        'size': 3,
        'res_name': null
      },
      {
        'id': '2',
        'name': 'today-sales',
        'title': null,
        'size': 3,
        'res_name': null
      },
      {
        'id': '3',
        'name': 'total-arrears',
        'title': null,
        'size': 3,
        'res_name': null
      },
      {
        'id': '4',
        'name': 'total-refund',
        'title': null,
        'size': 3,
        'res_name': null
      },
      {
        'id': '5',
        'name': 'sales',
        'title': null,
        'size': 12,
        'res_name': null
      },
      {
        'id': '6',
        'name': 'opera-monitor',
        'title': null,
        'size': 12,
        'res_name': null
      }
    ]);

    Mock.mock(/\/api\/dashboard\/[A-Za-z0-9-]+\/layout/, 'get', () => {
      return [
        {
          'id': '1',
          'name': 'total-sales',
          'title': null,
          'size': 3,
          'res_name': null
        },
        {
          'id': '2',
          'name': 'today-sales',
          'title': null,
          'size': 3,
          'res_name': null
        },
        {
          'id': '3',
          'name': 'total-arrears',
          'title': null,
          'size': 3,
          'res_name': null
        },
        {
          'id': '4',
          'name': 'total-refund',
          'title': null,
          'size': 3,
          'res_name': null
        },
        {
          'id': '5',
          'name': 'sales',
          'title': null,
          'size': 12,
          'res_name': null
        },
        {
          'id': '6',
          'name': 'opera-monitor',
          'title': null,
          'size': 12,
          'res_name': null
        }
      ];
    });

    Mock.mock(/\/api\/dashboard\/[A-Za-z0-9-]+\/layout/, 'put', () => {
      return [
        {
          'id': '1',
          'name': 'total-sales',
          'title': null,
          'size': 3,
          'res_name': null
        },
        {
          'id': '2',
          'name': 'today-sales',
          'title': null,
          'size': 3,
          'res_name': null
        },
        {
          'id': '3',
          'name': 'total-arrears',
          'title': null,
          'size': 3,
          'res_name': null
        },
        {
          'id': '4',
          'name': 'total-refund',
          'title': null,
          'size': 3,
          'res_name': null
        },
        {
          'id': '5',
          'name': 'sales',
          'title': null,
          'size': 12,
          'res_name': null
        },
        {
          'id': '6',
          'name': 'opera-monitor',
          'title': null,
          'size': 12,
          'res_name': null
        }
      ];
    });
  }
}
