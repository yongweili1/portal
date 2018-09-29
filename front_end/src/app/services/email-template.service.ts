import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';

import { Subject } from 'rxjs/Subject';
import { Observable } from 'rxjs';

import { TreeNode } from 'primeng/primeng';

import { AppConfigService } from '../app.config';
import { AuthHttp } from '../core/auth-http';
import { Page, PageRequest } from '../shared/models/pagination';
import { Email, EmailPageRequest, Param } from '../base/email-template/shared/email-template.model';

@Injectable()
export class EmailTemplateService {

  private dataChangedEvent = new Subject<Email>();

  constructor(
    private http: AuthHttp,
    private appConfig: AppConfigService) { }

  public isAddRefreshEvent(data: Email): void {
    this.dataChangedEvent.next(data);
  }

  public onAddRefresEvent(next?: (value: Email) => void): void {
    this.dataChangedEvent.subscribe({
      next
    });
  }

  public getEmailTemplate(pageRequest: EmailPageRequest): Observable<Page<Email>> {
    const params = new URLSearchParams();
    params.set('size', pageRequest.size.toString());
    params.set('page', pageRequest.page.toString());
    if (pageRequest.name) {
      params.set('name', pageRequest.name);
    } else {
      params.set('name', '');
    }
    return this.http
      .get(`${this.appConfig.apiUrl}/api/email-templates`, { search: params })
      .map((res) => res.json());
  }

  public getTemplateCode(id): Observable<Email> {
    return this.http.get(`${this.appConfig.apiUrl}/api/email-templates/${id}`)
      .map((res) => <Email>res.json());
  }

  public deleteEmailTemplate(id: string) {
    return this.http
      .delete(`${this.appConfig.apiUrl}/api/email-templates/${id}`)
      .map((res) => res.json());
  }

  public addEmailTemplate(email: Email): Observable<Email> {
    return this.http
      .post(`${this.appConfig.apiUrl}/api/email-templates`, email)
      .map((res) => res.json());
  }

  public updateEmailTemplate(email: Email) {
    const id = email.id;
    return this.http
      .put(`${this.appConfig.apiUrl}/api/email-templates/${id}`, email)
      .map((res) => res.json());
  }

  public getParameters(purpose): Observable<Param[]> {
    return this.http.get(`${this.appConfig.apiUrl}/api/template-param/params/${purpose}`)
      .map((res) => <Param[]>res.json());
  }

}
