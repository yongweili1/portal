import { Component, OnInit } from '@angular/core';
import { MenuItem, PacsPageRequest, PacsInfo } from '../pacs/shared/pacs.model'
import { AppConfigService } from '../../app.config';
import { Page, PageRequest } from '../../shared/models/pagination';
import { LazyLoadEvent } from 'primeng/primeng';
import {SelectItem} from 'primeng/api';
import { Router} from '@angular/router';
import { PacsService } from '../../services/pacs.service';

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
  pageModel: Page<PacsInfo>; 
  tablePageRows: number[] = [10, 15, 20, 50, 100];
  patientParam: PacsPageRequest = {} as PacsPageRequest;
 
  constructor(private pacsService: PacsService,private router:Router){

  this.cols = [
      {field: 'patientId', header: 'PATIENT ID'},
      {field: 'patientName', header: 'PATIENT NAME'},
      {field: 'gender', header: 'GENDER'},
  ];
  }

  public ngOnInit(): void {
    this.loadPacs();
  }

  public search (){
    this.loadPacs();
  }

  public loadPacs(event: LazyLoadEvent = { first: 0, rows: this.tablePageRows[1] }){
    this.requestPacs(
      Math.floor(event.first / (event.rows || this.tablePageRows[1])),
      event.rows || this.tablePageRows[1]
    )
  }

  private requestPacs( page: number = 0, size:number = this.tablePageRows[1]){
    this.pacsService.getPacs({
      page, 
      size,
      patientId:this.patientParam.patientId,
      patientName:this.patientParam.patientName,
      gender:this.patientParam.gender,
      modality:this.patientParam.modality,
    }).subscribe((data) => {
      this.pageModel = data;
      for (let i = 0; i < this.pageModel.numberOfElements; i++) {
        this.pageModel.content[i].no = i + 1;
        this.patientId = this.pageModel.content[i].patientId;
    }
    });
    return;
  }


  // public  download(){
  //   this.setDateParams();
  //   this.patientTemplateService.downloadExcel({
  //     patientId:this.patientParam.patientId,
  //     patientName:this.patientParam.patientName,
  //     gender:this.patientParam.gender,
  //     modality:this.patientParam.modality,
  //     studyDescription:this.patientParam.studyDescription,
  //     studyDate:this.rangeDate
  //   });
  // }

  public toContouring(id:string) {
    this.router.navigate(['/base/contouring'], {
      queryParams: {
          patientId: id,
      }
  });

  }
 
}
