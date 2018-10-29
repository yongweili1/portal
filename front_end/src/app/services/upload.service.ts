import { Observable } from 'rxjs';
import { share } from 'rxjs/operators';
import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { AuthHttp } from '../core/auth-http';
import {  HttpClient,HttpHeaders } from '@angular/common/http';
import { AppConfigService } from '../app.config';

@Injectable()
export class UploadService {
  progress;
  progressObserver;
  progress$:any;
  constructor(
    private http: AuthHttp,
    private appConfig: AppConfigService) {
      this.progress$ = Observable.create(observer => {
        this.progressObserver = observer
    });     
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

      if ('onprogress' in xhr.upload) {
        console.log('upload progress event supported');
      }
      else{
        console.log('upload progress event not supported');
      }

      xhr.upload.onprogress = (event) => {
        this.progress = Math.round(event.loaded / event.total * 100);
        console.log('上传比例'+this.progress)
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
