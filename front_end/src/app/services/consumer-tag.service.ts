import { Observable } from 'rxjs';
import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { AppConfigService } from '../app.config';
import { AuthHttp } from '../core/auth-http';
import { Page, PageRequest } from '../shared/models/pagination';


@Injectable()
export class ConsumerTagService {

  constructor(
    private http: AuthHttp,
    private appConfig: AppConfigService) { }

  public addConsumerTag(consumerTag) {
    return this.http
      .post(`${this.appConfig.apiUrl}/api/consumer-tag`, consumerTag)
      .map(
      (res) => {
        return res;
      },
      (error) => {
        console.error(error);
      });
  }

  public editConsumerTag(tagNo, consumerTag) {
    return this.http
      .put(`${this.appConfig.apiUrl}/api/consumer-tag/${tagNo}`, consumerTag)
      .map(
      (res) => {
        return res;
      },
      (error) => {
        console.error(error);
      });
  }

  public deleteTag(tagNo) {
    return this.http
      .delete(`${this.appConfig.apiUrl}/api/consumer-tag/${tagNo}`)
      .map((res) => res);
  }

  public getParentTags() {
    return this.http
      .get(`${this.appConfig.apiUrl}/api/consumer-tag/parents`)
      .map((res) => res.json());
  }

  public getTagMaxSn(upTagNo) {
    return this.http
      .get(`${this.appConfig.apiUrl}/api/consumer-tag/${upTagNo}/max-sn`)
      .map((res) => {
        return res.json();
      },
      (error) => {
        console.error(error);
      });
  }

}
