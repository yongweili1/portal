import { Component,OnInit } from '@angular/core';
import { MenuItem, PatientPageRequest, PatientTemplateInfo } from './shared/patient-template.model'
import { AppConfigService } from '../../app.config';
import { PatientTemplateService } from '../../services/patient-template.service';
import { Page, PageRequest } from '../../shared/models/pagination';
import { LazyLoadEvent } from 'primeng/primeng';
import {SelectItem} from 'primeng/api';
import { Router} from '@angular/router';
@Component({
  selector: 'mpt-palist',
  templateUrl: './patient-template.component.html',
  styleUrls: ['./patient-template.component.less']
})

export class PaListComponent implements OnInit {
    patientId:string;
    gender: MenuItem[] = [
      {label: 'Male'},
      {label: 'Female'},
      {label: 'Other'}
  ];
    modality:MenuItem[] = [
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
   
    constructor(private patientTemplateService: PatientTemplateService,private router:Router){

    this.cols = [
        {field: 'patientId', header: 'PATIENT ID'},
        {field: 'patientName', header: 'PATIENT NAME'},
        {field: 'gender', header: 'GENDER'},
        {field: 'modality', header: 'MODALITY'},
        {field: 'studyDescription', header: 'STUDYDESCRIPTION'}
    ];
    }

    public ngOnInit(): void {
      this.loadPatientTemplate();
    }

    public search (){
      this.loadPatientTemplate();
    }

    public loadPatientTemplate(event: LazyLoadEvent = { first: 0, rows: this.tablePageRows[1] }){
      this.requestPatientTemplate(
        Math.floor(event.first / (event.rows || this.tablePageRows[1])),
        event.rows || this.tablePageRows[1]
      )
    }

    private requestPatientTemplate( page: number = 0, size:number = this.tablePageRows[1]){
      this.setDateParams();
      console.log(this.patientParam.patientId);
      this.patientTemplateService.getPatientTemplate({
        page, 
        size,
        patientId:this.patientParam.patientId,
        patientName:this.patientParam.patientName,
        gender:this.patientParam.gender,
        modality:this.patientParam.modality,
        studyDescription: this.patientParam.studyDescription,
        studyDate:this.rangeDate  
      }).subscribe((data) => {
        this.pageModel = data;
        ///测试代码
        // this.pageModel.totalElements = 1;
        // this.pageModel.totalPages = 1;
        // this.pageModel.number = 1;
        // this.pageModel.size = 10;
        //  this.pageModel.content[0].patientId = '1';
        //  this.pageModel.content[0].patientName = 'cc';
        //  this.pageModel.content[0].studyDescription = 'cccc';
        ///
        for (let i = 0; i < this.pageModel.numberOfElements; i++) {
          this.pageModel.content[i].no = i + 1;
          this.patientId = this.pageModel.content[i].patientId;
      }
      });
      return;
    }

    private setDateParams() {
      if (this.patientParam !== null && this.patientParam.studyDate) {
        if (null !== this.patientParam.studyDate && undefined !== this.patientParam.studyDate && !this.patientParam.studyDate[1]) {
          this.rangeDate = [this.patientParam.studyDate[0].getTime(), this.patientParam.studyDate[0].getTime()];
        } else {
          this.rangeDate = [this.patientParam.studyDate[0].getTime(), this.patientParam.studyDate[1].getTime()];
        }
      }
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
