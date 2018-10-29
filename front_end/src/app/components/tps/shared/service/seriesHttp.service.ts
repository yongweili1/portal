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
import { AppConfigService } from '../../../../app.config';

@Injectable()
export class SeriesHttpService {
    headers: HttpHeaders; 
    constructor(
        private aj:AjaxService,
        private http: HttpClient,
        private appConfig: AppConfigService,
        private loadSeriesServiceMock:LoadSeriesServiceMock
    ) {
        const headers: HttpHeaders = new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded; charset=UTF-8' })
    }

getSeriesList(patientId:any):Observable<Array<any>>{
    const getParams = new HttpParams()
    .set('patientid', patientId);
    return this.http.get<Array<any>>(`${this.appConfig.apiUrl}/image/serids/`,{params : getParams});
}



/**
 * 从后端获取序列图片
 * @param seriesId 
 */
LoadVolume(seriesId:any):Observable<any>{
    const getParams = new HttpParams()
    .set('seriesuid', seriesId);
    return this.http.get(`${this.appConfig.apiUrl}/image/volumes/`,{params : getParams});
        
}

UnLoadVolume(seriesId:any):Observable<any>{
    const getParams = new HttpParams()
    .set('seriesuid', seriesId);
    return this.http.delete(`${this.appConfig.apiUrl}/image/volumes/`,{params : getParams});
        
}

GetSeries(seriesId:any,focus:any,display:any,width:any,height:any):Observable<any>{
    const getParams = new HttpParams()
     .set('focus_view',focus)
     .set('seriesuid',seriesId)
     .set('width', width)
     .set('height', height)
     .set('display_view', display);
    return this.http.get(`${this.appConfig.apiUrl}/image/images/`,{params : getParams});
    
}

GetSeriesPic(focus:any,display:any,delta:any,width:any,height:any):Observable<any>{
    const getParams = new HttpParams()
     .set('delta', delta)
     .set('width', width)
     .set('height', height)
     .set('focus_view', focus)
     .set('display_view', display);
    return this.aj.get(`${this.appConfig.apiUrl}/image/pages/`,{params : getParams});
    
}

GetLocatePic(focus:any,display:any,crossPoint:any):Observable<any>{
    const getParams = new HttpParams()
    .set('focus_view', focus)
    .set('display_view',display)
    .set('cross_point', crossPoint);
   return this.http.get(`${this.appConfig.apiUrl}/image/cross/`,{params : getParams});
}

GetZoomPic(focus:any,factor:any):Observable<any>{
    const getParams = new HttpParams()
    .set('focus_view', focus)
    .set('zoom_factor',factor);
   return this.http.get<any>(`${this.appConfig.apiUrl}/image/zooms/`,{params : getParams});
}

GetPanPic(focus:any,prePos:any,curPos:any):Observable<any>{
    const getParams = new HttpParams()
    .set('focus_view', focus)
    .set('pos_pre',prePos)
    .set('pos_cur',curPos);
   return this.http.get<any>(`${this.appConfig.apiUrl}/image/seats/`,{params : getParams});
}

GetRotatePic(focus:any,prePos:any,curPos:any):Observable<any>{
    const getParams = new HttpParams()
    .set('focus_view', focus)
    .set('pos_pre',prePos)
    .set('pos_cur',curPos);
   return this.http.get<any>(`${this.appConfig.apiUrl}/image/rotate/`,{params : getParams});
}

GetResetPic():Observable<any>{
   return this.http.get<any>(`${this.appConfig.apiUrl}/image/backs/`);
}

GetWindowPic(focus:any,ww_factor:any,wl_factor:any):Observable<any>{
    const getParams = new HttpParams()
    .set('focus_view', focus)
    .set('ww_factor',ww_factor)
    .set('wl_factor',wl_factor);
    return this.http.get<any>(`${this.appConfig.apiUrl}/image/windows/`,{params : getParams})
}

GetCenterPic():Observable<any>{
    return this.http.get<any>(`${this.appConfig.apiUrl}/image/center/`)
}

}
