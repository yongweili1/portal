import { Observable } from 'rxjs';
import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { AuthHttp } from '../core/auth-http';
import {  HttpClient,HttpHeaders } from '@angular/common/http';
import { AppConfigService } from '../app.config';

@Injectable()
export class UploadService {
  progress;
  progressObserver;
  constructor(
    private http: AuthHttp,
    private appConfig: AppConfigService) {
     
     }
    
  makeFileRequest (url: string,  files: File[]) {
    return Observable.create(observer => {
      let formData: FormData = new FormData(),
        xhr: XMLHttpRequest = new XMLHttpRequest();

      for (let i = 0; i < files.length; i++) {
        formData.append("a", files[i], files[i].name);
      }

      xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
          if (xhr.status === 200) {
            observer.next(JSON.parse(xhr.response));
            observer.complete();
          } else {
            observer.error(xhr.response);
          }
        }
      };

      xhr.upload.onprogress = (event) => {
        this.progress = Math.round(event.loaded / event.total * 100);

        this.progressObserver.next(this.progress);
      };

      xhr.open('POST', url, true);
      xhr.send(formData);
    });
  }

  // postUploadStatus(){
  //   this.http.post()
  // }
}
