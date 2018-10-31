import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { Observable, of } from 'rxjs';
import {  HttpClient,HttpHeaders } from '@angular/common/http';
import {HttpParams} from "@angular/common/http";
import { AjaxService } from "../services/ajax.service";

import { AppConfigService } from '../app.config';
import { AuthHttp } from '../core/auth-http';
import { Page, PageRequest } from '../shared/models/pagination';
import {
  PacsPageRequest,
  PacsInfoRequest,
} from '../components/pacs/shared/pacs.model';
import {ToastService} from '../core/toast.service';

@Injectable()
export class PacsService {
  headers: HttpHeaders; 

  constructor(
    private http: HttpClient,
    private appConfig: AppConfigService,
    private messageService: ToastService,
    private aj :AjaxService
  ) {
    this.headers = new HttpHeaders({
      'Content-Type':  'application/json',
      'withCredentials': 'true'
     })
   }

   public getPacs(pageRequest: PacsPageRequest): Observable<Page<PacsPageRequest>> {
    const searchParams = new HttpParams()
    .set('size', pageRequest.size.toString())
    .set('page', pageRequest.page.toString())
    .set('patientId', pageRequest.patientId)
    .set('patientName', pageRequest.patientName)
    .set('gender', pageRequest.gender)
    .set('modality', pageRequest.modality);
    console.log(this.headers);
    return this.http
      .get<Page<PacsPageRequest>>(`${this.appConfig.apiUrl}/pacs/pacspatient/`,{headers:this.headers, params:searchParams});  
  }
}

const httpOptions = {
  headers: new HttpHeaders({ 'Content-Type': 'application/json' })
};
