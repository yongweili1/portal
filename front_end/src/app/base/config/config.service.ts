import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { Observable } from 'rxjs';

import { AppConfigService } from '../../app.config';
import { AuthHttp } from '../../core/auth-http';

import { ConfItem } from './config.model';

@Injectable()
export class ConfigService {

  constructor(
    private http: AuthHttp,
    private appConfig: AppConfigService) { }

  public all(): Observable<ConfItem[]> {
    return this.http.get(`${this.appConfig.apiUrl}/api/config`)
      .map((res) => <ConfItem[]> res.json());
  }

}
