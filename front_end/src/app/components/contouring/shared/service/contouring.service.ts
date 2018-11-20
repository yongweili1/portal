import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { Observable } from 'rxjs';
import { Subject } from 'rxjs/Subject';

import { AppConfigService } from '../../../../app.config';
import { AuthHttp } from '../../../../core/auth-http';
import { HttpClient, HttpHeaders, HttpResponse } from '@angular/common/http';
import { HttpParams } from "@angular/common/http";
import { ContourModel } from '../model/contour.model';


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

  saveContour(dto: ContourModel): Observable<any> {
    return this.http.post<ContourModel>(`${this.appConfig.apiUrl}/contour/contour/`, dto, this.options);
  }

  deleteContours(dto: ContourModel): Observable<any> {
    const getParams = new HttpParams()
      .set('roi_uid', dto['roi_uid'])
      .set('slice_index', dto['slice_index']);
    return this.http.delete(`${this.appConfig.apiUrl}/contour/contour/`, { params: getParams });
  }

  getContours(roi_uid, slice_index): Observable<any> {
    const getParams = new HttpParams()
      .set('roi_uid', roi_uid)
      .set('slice_index', slice_index);
    return this.http.get(`${this.appConfig.apiUrl}/contour/contour/`, { params: getParams })
  }
}
