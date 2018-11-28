import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/Observable';
import { AppConfigService } from '../../../../app.config';
import { AjaxService } from "../../../../services/ajax.service";
import { retry } from 'rxjs/operators';

@Injectable()
export class SeriesHttpService {
    headers: HttpHeaders;
    constructor(
        private aj: AjaxService,
        private http: HttpClient,
        private appConfig: AppConfigService,
    ) {
        const headers: HttpHeaders = new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded; charset=UTF-8' })
    }

    getSeriesList(patientId: any): Observable<Array<any>> {
        const getParams = new HttpParams()
            .set('patientid', patientId);
        return this.http.get<Array<any>>(`${this.appConfig.apiUrl}/image/serids/`, { params: getParams });
    }

    /**
     * 从后端获取序列图片
     * @param seriesId
     */
    LoadVolume(seriesId: any): Observable<any> {
        const getParams = new HttpParams()
            .set('seriesuid', seriesId);
        return this.http.get(`${this.appConfig.apiUrl}/image/volumes/`, { params: getParams }).pipe(retry(1));
    }

    UnLoadVolume(seriesId: any): Observable<any> {
        const getParams = new HttpParams()
            .set('seriesuid', seriesId);
        return this.http.delete(`${this.appConfig.apiUrl}/image/volumes/`, { params: getParams });

    }

    GetSeries(): Observable<any> {
        return this.http.get(`${this.appConfig.apiUrl}/image/images/`);
    }

    GetSeriesPic(focus: any, delta: any): Observable<any> {
        const getParams = new HttpParams()
            .set('delta', delta)
            .set('focus_view', focus);
        return this.aj.get(`${this.appConfig.apiUrl}/image/pages/`, { params: getParams });

    }

    GetLocatePic(focus: any, crossPoint: any): Observable<any> {
        const getParams = new HttpParams()
            .set('focus_view', focus)
            .set('cross_point', crossPoint);
        return this.http.get(`${this.appConfig.apiUrl}/image/cross/`, { params: getParams });
    }

    GetZoomPic(focus: any, factor: any): Observable<any> {
        const getParams = new HttpParams()
            .set('focus_view', focus)
            .set('zoom_factor', factor);
        return this.http.get<any>(`${this.appConfig.apiUrl}/image/zooms/`, { params: getParams });
    }

    GetPanPic(focus: any, prePos: any, curPos: any): Observable<any> {
        const getParams = new HttpParams()
            .set('focus_view', focus)
            .set('pos_pre', prePos)
            .set('pos_cur', curPos);
        return this.http.get<any>(`${this.appConfig.apiUrl}/image/seats/`, { params: getParams });
    }

    GetRotatePic(focus: any, prePos: any, curPos: any): Observable<any> {
        const getParams = new HttpParams()
            .set('focus_view', focus)
            .set('pos_pre', prePos)
            .set('pos_cur', curPos);
        return this.http.get<any>(`${this.appConfig.apiUrl}/image/rotate/`, { params: getParams });
    }

    GetResetPic(): Observable<any> {
        return this.http.get<any>(`${this.appConfig.apiUrl}/image/backs/`);
    }

    GetWindowPic(ww: any, wl: any): Observable<any> {
        const getParams = new HttpParams()
            .set('ww', ww)
            .set('wl', wl)
        return this.http.get<any>(`${this.appConfig.apiUrl}/image/windows/`, { params: getParams })
    }

    GetCenterPic(): Observable<any> {
        return this.http.get<any>(`${this.appConfig.apiUrl}/image/center/`)
    }
}
