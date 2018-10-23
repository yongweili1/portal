import { Injectable } from '@angular/core';
import {  HttpClient,HttpHeaders } from '@angular/common/http';
import { AuthHttp } from '../../../../core/auth-http';
import { SecurityService } from '../../../../services/security.service';
import { Observable } from 'rxjs/Observable';
import { StorageService } from './storage.service';
import {HttpParams} from "@angular/common/http";
import {LoadSeriesServiceMock} from '../../../../mocks/load-series-service.mock'
import { ajax } from "rxjs/ajax";
import { AjaxService } from "../../../../services/ajax.service";

@Injectable()
export class SeriesHttpService {
    headers: HttpHeaders; 
    constructor(
        private aj:AjaxService,
        private http: HttpClient,
        private loadSeriesServiceMock:LoadSeriesServiceMock
    ) {
        const headers: HttpHeaders = new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded; charset=UTF-8' })
    }

getSeriesList(patientId:any):Observable<Array<any>>{
    const getParams = new HttpParams()
    .set('patientid', patientId);
    return this.http.get<Array<any>>('http://10.9.19.24:8000/image/serids/',{params : getParams});
}



/**
 * 从后端获取序列图片
 * @param seriesId 
 */
LoadSeries(seriesId:any):Observable<any>{
    const getParams = new HttpParams()
    .set('seriesuid', seriesId);
    return this.http.get('http://10.9.19.24:8000/image/volumes/',{params : getParams});
        
}

GetSeries(seriesId:any,focus:any,display:any,width:any,height:any):Observable<any>{
    const getParams = new HttpParams()
     .set('focus_view',focus)
     .set('seriesuid',seriesId)
     .set('width', width)
     .set('height', height)
     .set('display_view', display);
    //return this.http.get('http://10.9.19.139:8000/image/images/',{params : getParams});
    return this.http.get('http://10.9.19.24:8000/image/images/',{params : getParams});
    
}

GetSeriesPic(focus:any,display:any,delta:any,width:any,height:any):Observable<any>{
    const getParams = new HttpParams()
     .set('delta', delta)
     .set('width', width)
     .set('height', height)
     .set('focus_view', focus)
     .set('display_view', display);
    //return this.http.get('http://10.9.19.139:8000/image/pages/',{params : getParams});
    return this.http.get('http://10.9.19.24:8000/image/pages/',{params : getParams});
    
}

GetLocatePic(focus:any,display:any,crossPoint:any):Observable<any>{
    const getParams = new HttpParams()
    .set('focus_view', focus)
    .set('display_view',display)
    .set('cross_point', crossPoint);
   //return this.http.get('http://10.9.19.139:8000/image/pages/',{params : getParams});
   return this.http.get('http://10.9.19.24:8000/image/cross/',{params : getParams});
}



}
