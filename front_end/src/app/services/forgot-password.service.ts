import { Observable } from 'rxjs';
import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { AuthHttp } from '../core/auth-http';
import { AppConfigService } from '../app.config';
import { ForgotPwdInfo } from '../models/forgot-password.model';
@Injectable()
export class ForgotPasswordService {

  constructor(
    private http: AuthHttp,
    private appConfig: AppConfigService) { }

 public submitFotgotInfo(forgotPwdInfo: ForgotPwdInfo): Observable<ForgotPwdInfo> {
    return this.http.post(`${this.appConfig.apiUrl}/api/forgot-password`, forgotPwdInfo)
      .map((res) => res.json());
  }

  public submitResetInfo(param): Observable<ForgotPwdInfo> {
    return this.http
      .post(`${this.appConfig.apiUrl}/api/forgot-password/${param.id}/reset-password`, param.password)
      .map((res) => res.json());
  }

}
