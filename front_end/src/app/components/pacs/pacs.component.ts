import { Component, OnInit } from '@angular/core';
import { MenuItem, PacsPageRequest, PacsInfo } from '../pacs/shared/pacs.model'
import { AppConfigService } from '../../app.config';
import { Page, PageRequest } from '../../shared/models/pagination';
import { LazyLoadEvent } from 'primeng/primeng';
import {SelectItem} from 'primeng/api';
import { Router} from '@angular/router';
import { PacsService } from '../../services/pacs.service';
import { MessageService } from 'primeng/api';
import { forEach } from '@angular/router/src/utils/collection';
import { element } from 'protractor';

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
    {label: 'All'}
];
  public modality:MenuItem[] = [
    {label: 'MR'},
    {label: 'CT'},
    {label: 'CR'},
    {label: 'DX'},
    {label: 'All'}
  ];
  cols:any[]= [];
  pageModel: Page<PacsInfo>; 
  selectedPageModel: any;
  tablePageRows: number[] = [10, 15, 20, 50, 100];
  patientParam: PacsPageRequest = {} as PacsPageRequest;
 
  constructor(
    private pacsService: PacsService,
    private router:Router,
    private priMessageService: MessageService
  ){

  this.cols = [
      {field: 'patientId', header: 'PATIENT ID'},
      {field: 'patientName', header: 'PATIENT NAME'},
      {field: 'patientAge', header: 'AGE'},
      {field: 'gender', header: 'GENDER'},
      {field: 'modality', header: 'MODALITY'}
  ];
  }

  public ngOnInit(): void {
    this.loadPacs();
    this.selectedPageModel = {};
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
      patientAge:this.patientParam.patientAge,
      gender:JSON.stringify(this.patientParam.gender),  //JSON.stringify(this.patientParam.gender)
      modality:JSON.stringify(this.patientParam.modality),
    }).subscribe((data) => {
      this.pageModel = data;
      for (let i = 0; i < this.pageModel.numberOfElements; i++) {
        this.pageModel.content[i].no = i + 1;
        this.patientId = this.pageModel.content[i].patientId;
    }
    });
    
    return;
  }

  private download(){
    let patientIdArray = []
    let patientIdString = ""
    this.selectedPageModel.content.forEach(element =>{
      patientIdArray.push(element.patientId);
    });
     patientIdString = patientIdArray.toString();
    this.pacsService.getDownloadPacs({
      patientId: patientIdString
    }).subscribe(result =>{
      if(result == "Download Success"){
        this.priMessageService.add({ severity: 'success', detail: 'download succeed' });
      }
      else{
        console.log("==== download failed ===")
        this.priMessageService.add({ severity: 'error', detail: 'download failed' });
      }
    },(error)=>{
      this.priMessageService.add({ severity: 'error', detail: 'download failed' });
    })
  }

}
