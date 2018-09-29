import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';

import { Subject } from 'rxjs/Subject';
import { Observable } from 'rxjs';

import { TreeNode } from 'primeng/primeng';

import { AppConfigService } from '../app.config';
import { AuthHttp } from '../core/auth-http';
import { Page, PageRequest } from '../shared/models/pagination';
import { Sms, SmsPageRequest, Param } from '../base/sms-template/shared/sms-template.model';

@Injectable()
export class SmsTemplateService {

  private dataChangedEvent = new Subject<Sms>();

  constructor(
    private http: AuthHttp,
    private appConfig: AppConfigService) { }

  public isAddRefreshEvent(data: Sms): void {
    this.dataChangedEvent.next(data);
  }

  public onAddRefresEvent(next?: (value: Sms) => void): void {
    this.dataChangedEvent.subscribe({
      next
    });
  }

  public getSmsTemplate(pageRequest: SmsPageRequest): Observable<Page<Sms>> {
    const params = new URLSearchParams();
    params.set('size', pageRequest.size.toString());
    params.set('page', pageRequest.page.toString());
    if (pageRequest.name) {
      params.set('name', pageRequest.name);
    } else {
      params.set('name', '');
    }
    return this.http
      .get(`${this.appConfig.apiUrl}/api/sms-templates`, { search: params })
      .map((res) => res.json());
  }

  public getTemplateCode(id: string): Observable<Sms> {
    return this.http.get(`${this.appConfig.apiUrl}/api/sms-templates/${id}`)
      .map((res) => <Sms>res.json());
  }

  public deleteSmsTemplate(id: string) {
    return this.http
      .delete(`${this.appConfig.apiUrl}/api/sms-templates/${id}`)
      .map((res) => res.json());
  }

  public addSmsTemplate(sms: Sms): Observable<Sms> {
    return this.http
      .post(`${this.appConfig.apiUrl}/api/sms-templates`, sms)
      .map((res) => res.json());
  }

  public updateSmsTemplate(sms: Sms) {
    const id = sms.id;
    return this.http
      .put(`${this.appConfig.apiUrl}/api/sms-templates/${id}`, sms)
      .map((res) => res.json());
  }

  public getParameters(purpose): Observable<Param[]> {
    return this.http.get(`${this.appConfig.apiUrl}/api/template-param/params/${purpose}`)
      .map((res) => <Param[]>res.json());
  }

}
