import { Injectable } from '@angular/core';
import { Observable} from 'rxjs';
import { HttpParams,HttpHeaders} from "@angular/common/http";


declare var $: any;

@Injectable({
  providedIn: 'root'
})
export class AjaxService {

  constructor() { }

  get<T>(url: string,options?: {
    headers?: HttpHeaders | {
        [header: string]: string | string[];
    };
    observe?: 'body';
    params?: HttpParams | {
        [param: string]: string | string[];
    };
    reportProgress?: boolean;
    responseType?: 'json';
    withCredentials?: boolean;
    }): Observable<T>{
    let myPara : string = "";
    let myHeader : string = "";
    if(options && options.params)
    {
      myPara = options.params.toString();
    }
    if(options && options.headers)
    {
      myHeader = options.headers.toString();
    }
    let obs = new Observable<T>();
    obs = Observable.fromPromise(
      $.ajax({  
        type: "GET",  
        url: url,
        headers:myHeader,
        data: myPara,
        dataType: 'json',
        })
    )
    return obs;
  }
}
