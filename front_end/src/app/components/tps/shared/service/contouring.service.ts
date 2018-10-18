import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { Observable } from 'rxjs';
import { Subject } from 'rxjs/Subject';

import { AppConfigService } from '../../../../app.config';
import { AuthHttp } from '../../../../core/auth-http';
import { HttpClient } from '@angular/common/http';
import {HttpParams} from "@angular/common/http";


@Injectable()
export class ContouringService {

  constructor(
    private http: HttpClient,
    private appConfig: AppConfigService
  ) { }

  Macro(macro_status:any){
    const getParams = new HttpParams()
    .set('macro_status',macro_status);
    return this.http.get("http://10.9.19.139:8000/image/macro/",{params:getParams});
  }

  noticeSize(canvasSize){
    return this.http.post("http://10.9.19.139:8000/image/resize/",canvasSize);
  }

}
