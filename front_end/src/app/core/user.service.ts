import { Observable } from 'rxjs';
import { Injectable } from '@angular/core';
import { Response, URLSearchParams } from '@angular/http';
import { AppConfigService } from '../app.config';
import { AuthHttp } from './auth-http';

export interface User {
  userId?: string;
  userName?: string;
  realName?: string;
  email?: string;
  telephone?: string;
  wechat?: string;
  description?: string;

  deptId?: string;

  isManage?: number;
  isSystem?: number;

  enabled?: number;
  isDelete?: number;

  isLock?: number;
  lockTime?: Date;

  failCount?: number;
  passwordUpdateTime?: Date;

  attr1?: string;
  attr2?: string;
  attr3?: string;
  attr4?: string;
  attr5?: string;

}

@Injectable()
export class UserService {

  constructor(
    private http: AuthHttp,
    private appConfig: AppConfigService) {
  }

  public me(): Observable<User> {
    return this.http.get(`${this.appConfig.apiUrl}/api/user-info`)
      .map((res) => res.json());
  }

  public updateMe(user: User, password: string): Observable<Response> {
    let params = new URLSearchParams();
    params.set('password', password);
    return this.http.put(`${this.appConfig.apiUrl}/api/user-info`, user, { search: params });
  }

  public changePassword(oldPassword: string, newPassword: string): Observable<Response> {
    let params = new URLSearchParams();
    params.set('oldPassword', oldPassword);
    params.set('newPassword', newPassword);
    return this.http.post(`${this.appConfig.apiUrl}/api/change-password`, {}, { search: params });
  }

}
