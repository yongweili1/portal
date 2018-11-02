import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { Observable } from 'rxjs';
import { Subject } from 'rxjs/Subject';

import { AppConfigService } from '../../../../app.config';
import { AuthHttp } from '../../../../core/auth-http';
import { HttpClient, HttpHeaders, HttpResponse } from '@angular/common/http';
import { HttpParams } from "@angular/common/http";
import { ContourDto } from '../dtos/contour_dto';


@Injectable()
export class ContouringService {

  headers: HttpHeaders
  options: any

  constructor(
    private http: HttpClient,
    private appConfig: AppConfigService,
  ) {
    this.headers = new HttpHeaders();
    this.headers.append('Content-Type', 'application/json');
    this.headers.append('Accept', 'application/json');

    this.options = {
      headers: this.headers,
      observe: "response",
    }
  }

  Macro(macro_status: any) {
    const getParams = new HttpParams()
      .set('macro_status', macro_status);
    return this.http.get(`${this.appConfig.apiUrl}/image/macro/`, { params: getParams });
  }

  noticeSize(canvasSize): Observable<any> {
    return this.http.post<any>(`${this.appConfig.apiUrl}/image/size/`, canvasSize, this.options);
  }

  postContour(dto: ContourDto) {
    let url = `${this.appConfig.apiUrl}/image/contour/`;

    return this.http.post<any>(url, dto).map((response: HttpResponse<any>) => {
      if (response.status == 200) {
        const _responseText = response.body.result;
        return _responseText;
      } else {
        return false;
      }
    });
  }
}
