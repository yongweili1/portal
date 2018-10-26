import { Component, OnInit } from '@angular/core';
import { MenuItem, PatientPageRequest, PatientTemplateInfo } from '../patient-template/shared/patient-template.model'
import { AppConfigService } from '../../app.config';
import { PatientTemplateService } from '../../services/patient-template.service';
import { Page, PageRequest } from '../../shared/models/pagination';
import { LazyLoadEvent } from 'primeng/primeng';
import {SelectItem} from 'primeng/api';
import { Router} from '@angular/router';

import {PanelModule} from 'primeng/panel';

@Component({
  selector: 'mpt-pacs',
  templateUrl: './pacs.component.html',
  styleUrls: ['./pacs.component.less']
})
export class PacsComponent implements OnInit {
  patientId:string;

  public gender: MenuItem[] = [
    {label: 'Male'},
    {label: 'Female'},
    {label: 'Other'}
];
  public modality:MenuItem[] = [
    {label: 'MR'},
    {label: 'CT'},
    {label: 'CR'},
    {label: 'DX'}
  ];
  cols:any[]= [];
  rangeDate = [];
  pageModel: Page<PatientTemplateInfo>; 
  tablePageRows: number[] = [10, 15, 20, 50, 100];
  patientParam: PatientPageRequest = {} as PatientPageRequest;
 
  constructor(private router:Router){

  this.cols = [
      {field: 'patientId', header: 'PATIENT ID'},
      {field: 'patientName', header: 'PATIENT NAME'},
      {field: 'gender', header: 'GENDER'},
  ];
  }

  public ngOnInit(): void {

  }

 
  

}
