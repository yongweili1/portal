import { Observable } from 'rxjs';
import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { AppConfigService } from '../../app.config';
import { AuthHttp } from '../../core/auth-http';
export interface User {
  userName?: string;
  oldPassword?: string;
  newPassword?: string;
  confirmPassword?: string;
}

@Injectable()
export class RenewPasswordService {

  constructor(
    private http: AuthHttp,
    private appConfig: AppConfigService) { }

  public submitUserInfo(user: User) {
    return this.http.post(`${this.appConfig.apiUrl}/api/renew-password`, user)
      .map((res) => res.json());
  }

}
