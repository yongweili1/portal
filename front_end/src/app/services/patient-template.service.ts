import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { Observable, of } from 'rxjs';
import {  HttpClient,HttpHeaders } from '@angular/common/http';
import {HttpParams} from "@angular/common/http";
import { AjaxService } from "../services/ajax.service";

import { AppConfigService } from '../app.config';
import { AuthHttp } from '../core/auth-http';
import { Page, PageRequest } from '../shared/models/pagination';
import { PatientTemplateServiceMock } from '../mocks/patient-template-service.mock'
import {
  PatientPageRequest,
  PatientTemplateInfoRequest,
} from '../components/patient-template/shared/patient-template.model';
import {ToastService} from '../core/toast.service';

@Injectable()
export class PatientTemplateService {

  constructor(
    private http: HttpClient,
    private appConfig: AppConfigService,
    private messageService: ToastService,
    private patientService: PatientTemplateServiceMock,
    private aj :AjaxService
) { }

  public getPatientTemplate(pageRequest: PatientPageRequest): Observable<Page<PatientPageRequest>> {
    const searchParams = new HttpParams()
    .set('size', pageRequest.size.toString())
    .set('page', pageRequest.page.toString())
    .set('patientId', pageRequest.patientId)
    .set('patientName', pageRequest.patientName)
    .set('gender', pageRequest.gender)
    .set('modality', pageRequest.modality)
    .set('studyDate', pageRequest.studyDate.toString());
    // return this.http
    //   .get<Page<PatientPageRequest>>('http://10.9.19.139:8000/patientinformations/patinfolist/',{params:searchParams});
      // return this.http
      // .get<Page<PatientPageRequest>>('http://127.0.0.1:8000/patientinformations/patinfolist/',{params:searchParams});
    // return this.http
    //   .get<Page<PatientPageRequest>>(`${this.appConfig.apiUrl}/patientinformations/patinfolist/`,{params:searchParams});
    return this.http
    .get<Page<PatientPageRequest>>('http://10.9.19.24:8000/patientinformations/patinfolist/');
  }


  // downloadExcel(patientTemplate: PatientTemplateInfoRequest) {
  //   const params = new URLSearchParams();
  //   params.set('patientId', patientTemplate.patientId);
  //   params.set('patientName', patientTemplate.patientName);
  //   params.set('gender', patientTemplate.gender);
  //   params.set('modality', patientTemplate.modality);
  //   params.set('studyDescription',patientTemplate.studyDescription);
  //   params.set('studyDate', patientTemplate.studyDate.toString());
  //   const url = `${this.appConfig.apiUrl}/api/patient-template/download`;
  //   this.http.get(url, {responseType: 3, search: params})
  //     .subscribe((res) => {
  //       let type;
  //       type = 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet';
  //       let blob = new Blob([res.json()], { type });
  //       let fileName = new Date().getTime() + '.xlsx';
  //       let a = document.createElement('a');
  //       document.body.appendChild(a);
  //       a.download = fileName;
  //       a.href = URL.createObjectURL(blob);
  //       a.click();
  //     }, (error) => {
  //       this.messageService.error(error);
  //     }, () => {
  //     });
  // }
}
const httpOptions = {
  headers: new HttpHeaders({ 'Content-Type': 'application/json' })
};

