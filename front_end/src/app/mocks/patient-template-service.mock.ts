import { Injectable } from '@angular/core';

declare var Mock: any;
const Random = Mock.Random;

@Injectable()
export class PatientTemplateServiceMock {
 
  constructor() {
    Mock.mock(/\/api\/patient-template/, 'get', () => {
      return {
        'content': [
          {
            'patientId': '402880',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'CT',
            'studyDescription':'this is ...'
          },
          {
            'patientId': '456hgh',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'MR',
            'studyDescription':'this is ...'
          },
          {
            'patientId': '879ij',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'CT',
            'studyDescription':'this is ...'
          },
          {
            'patientId': 'gdb43',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'CT',
            'studyDescription':'this is ...'
          },
          {
            'patientId': '4bgf5',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'CR',
            'studyDescription':'this is ...'
          },
          {
            'patientId': 'h5445',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'CT',
            'studyDescription':'this is ...'
          },
          {
            'patientId': '4hg56',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'CT',
            'studyDescription':'this is ...'
          },
          {
            'patientId': 'fd80',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'DX',
            'studyDescription':'this is ...'
          },
          {
            'patientId': '5680',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'MR',
            'studyDescription':'this is ...'
          },
          {
            'patientId': 'bggd580',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'CT',
            'studyDescription':'this is ...'
          },
          {
            'patientId': '34thtr',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'CR',
            'studyDescription':'this is ...'
          },
          {
            'patientId': 'teg80',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'CT',
            'studyDescription':'this is ...'
          },
          {
            'patientId': '4tgt80',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'CT',
            'studyDescription':'this is ...'
          },
          {
            'patientId': '4et0',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'MR',
            'studyDescription':'this is ...'
          },
          {
            'patientId': '32880',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'CT',
            'studyDescription':'this is ...'
          }
        ],
        'first': true, //第一页
        // 'last': false, //最后一页
        'totalPages': 1, //页数
        'totalElements': 15, //总条数
        'number': 0, //第几页
        'size': 15, //每页条数
        'numberOfElements': 15, //当页条数
        // 'sort': [
        //   {
        //     'direction': 'DESC',
        //     'property': 'templateId',
        //     'ignoreCase': false,
        //     'nullHandling': 'NATIVE',
        //     'ascending': false,
        //     'descending': true
        //   }
        // ],
      };
    });

    Mock.mock(/\/api\/patient-template\/[A-Za-z0-9-]+/, 'get', () => {
      return {
        'patientId': '402880',
            'patientName': 'DR_HAND',
            'gender': 'Male',
            'modality':'CT'
      };
    });

    Mock.mock(/\/api\/patient-template/, 'post', (option) => {
      return true;
    });

  }
}
