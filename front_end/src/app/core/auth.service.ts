import { Observable } from 'rxjs';
import { Subject } from 'rxjs/Subject';
import { Injectable } from '@angular/core';
import { Http, Response, URLSearchParams } from '@angular/http';
import { AppConfigService } from '../app.config'; 
import { AuthHttp, tokenNotExpired, saveToken, removeToken } from './auth-http';

import { AuthServiceMock } from '../mocks/auth-service.mock'
@Injectable()
export class AuthService {

  private authEvents: Subject<AuthEvent>;

  private cachePermissionData = {};

  constructor(
    private http: Http,
    private authService: AuthServiceMock,
    private authHttp: AuthHttp,
    private appConfig: AppConfigService) {
    this.authEvents = new Subject<AuthEvent>();
  }

  public login(username: string, password: string): Observable<Response> {
    const body = {
      username,
      password,
    };

    return this.http.post(`${this.authService}/api/auth`, body).do((resp: Response) => {
      saveToken(resp.json().token);
      this.authEvents.next(new DidLogin());
    });
  }

  public logout(): void {
    this.cachePermissionData = {};
    removeToken();
    this.authEvents.next(new DidLogout());
  }

  public loggedIn(): boolean {
    return tokenNotExpired();
  }

  get events(): Observable<AuthEvent> {
    return this.authEvents.asObservable();
  }

  public isAdminUser(userName: string): boolean {
    return 'admin' === userName;
  }

}

export class DidLogin { }

export class DidLogout { }

export type AuthEvent = DidLogin | DidLogout;

