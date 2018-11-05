import { Injectable } from '@angular/core';

import { Observable, of } from 'rxjs';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { HttpParams } from "@angular/common/http";

import { AppConfigService } from '../app.config';

@Injectable({
  providedIn: 'root'
})
export class TaskService {

  constructor(
    private http: HttpClient,
    private appConfig: AppConfigService,
  ) { }

  getHardWareStatus():Observable<any>{
      return this.http.get<any>(`${this.appConfig.apiUrl}/task/hardwarestatus/`);
  }
}
