import { Observable } from 'rxjs';
import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { AuthHttp } from '../core/auth-http';
import { AppConfigService } from '../app.config';
import { ForgotPwdInfo } from '../models/forgot-password.model';
import { VerifyCodeServiceMock } from '../mocks/verify-code-service.mock'
@Injectable()
export class VerifyCodeService {

  constructor(
    private http: AuthHttp,
    private appConfig: AppConfigService,
    private verifyCodeService: VerifyCodeServiceMock
  ) { }

  public getCode() {
    return this.http.get(`${this.verifyCodeService}/api/verify-code`)
      .map((res) => res.json());
  }

  public checkCode(forgotPwdInfo: ForgotPwdInfo): Observable<ForgotPwdInfo> {
    return this.http
      .post(`${this.verifyCodeService}/api/verify-code/${forgotPwdInfo.id}`, forgotPwdInfo)
      .map((res) => res.json());
  }

}
  