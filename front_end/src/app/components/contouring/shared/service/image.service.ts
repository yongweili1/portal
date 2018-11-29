import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/Observable';
import { AppConfigService } from '../../../../app.config';
import { retry } from 'rxjs/operators';

@Injectable()
export class ImageService {
    headers: HttpHeaders;
    options: any;

    constructor(
        private http: HttpClient,
        private appConfig: AppConfigService,
    ) {
        this.headers = new HttpHeaders();
        this.headers.append('Content-Type', 'application/json');
        this.headers.append('Accept', 'application/json');

        this.options = {
            headers: this.headers,
            observe: 'response',
        };
    }

    getSeriesList(patientId: any): Observable<any> {
        const getParams = new HttpParams()
            .set('patientid', patientId);
        return this.http.get<Array<any>>(`${this.appConfig.apiUrl}/image/serids/`, { params: getParams });
    }

    updateCanvasSize(canvasSize): Observable<any> {
        return this.http.post<any>(`${this.appConfig.apiUrl}/image/size/`, canvasSize, this.options)
            .map(x => x['body']);
    }

    /**
     * 从后端获取序列图片
     * @param seriesId
     */
    loadVolume(seriesId: any): Observable<any> {
        const getParams = new HttpParams()
            .set('seriesuid', seriesId);
        return this.http.get(`${this.appConfig.apiUrl}/image/volumes/`, { params: getParams })
            .map(response => {
                response['data'] = JSON.parse(response['data']);
                return response;
            })
            .pipe(retry(1));
    }

    unloadVolume(seriesId: any): Observable<any> {
        const getParams = new HttpParams()
            .set('seriesuid', seriesId);
        return this.http.delete(`${this.appConfig.apiUrl}/image/volumes/`, { params: getParams });
    }

    GetSeries(): Observable<any> {
        return this.http.get(`${this.appConfig.apiUrl}/image/images/`)
            .map(response => {
                response['data'] = JSON.parse(response['data']);
                return response;
            });
    }

    page(focus: any, delta: any): Observable<any> {
        const getParams = new HttpParams()
            .set('delta', delta)
            .set('focus_view', focus);
        return this.http.get(`${this.appConfig.apiUrl}/image/page/`, { params: getParams })
            .map(response => {
                response['data'] = JSON.parse(response['data']);
                return response;
            });
    }

    locate(focus: any, crossPoint: any): Observable<any> {
        const getParams = new HttpParams()
            .set('focus_view', focus)
            .set('cross_point', crossPoint);
        return this.http.get(`${this.appConfig.apiUrl}/image/cross/`, { params: getParams })
            .map(response => {
                response['data'] = JSON.parse(response['data']);
                return response;
            });
    }

    zoom(focus: any, factor: any): Observable<any> {
        const getParams = new HttpParams()
            .set('focus_view', focus)
            .set('zoom_factor', factor);
        return this.http.get<any>(`${this.appConfig.apiUrl}/image/zoom/`, { params: getParams })
            .map(response => {
                response['data'] = JSON.parse(response['data']);
                return response;
            });
    }

    pan(focus: any, prePos: any, curPos: any): Observable<any> {
        const getParams = new HttpParams()
            .set('focus_view', focus)
            .set('pos_pre', prePos)
            .set('pos_cur', curPos);
        return this.http.get<any>(`${this.appConfig.apiUrl}/image/pan/`, { params: getParams })
            .map(response => {
                response['data'] = JSON.parse(response['data']);
                return response;
            });
    }

    rotate(focus: any, prePos: any, curPos: any): Observable<any> {
        const getParams = new HttpParams()
            .set('focus_view', focus)
            .set('pos_pre', prePos)
            .set('pos_cur', curPos);
        return this.http.get<any>(`${this.appConfig.apiUrl}/image/rotate/`, { params: getParams })
            .map(response => {
                response['data'] = JSON.parse(response['data']);
                return response;
            });
    }

    reset(): Observable<any> {
        return this.http.get<any>(`${this.appConfig.apiUrl}/image/reset/`)
            .map(response => {
                response['data'] = JSON.parse(response['data']);
                return response;
            });
    }

    wwwl(ww: any, wl: any): Observable<any> {
        const getParams = new HttpParams()
            .set('ww', ww)
            .set('wl', wl);
        return this.http.get<any>(`${this.appConfig.apiUrl}/image/windows/`, { params: getParams })
            .map(response => {
                response['data'] = JSON.parse(response['data']);
                return response;
            });
    }

    center(): Observable<any> {
        return this.http.get<any>(`${this.appConfig.apiUrl}/image/center/`)
            .map(response => {
                response['data'] = JSON.parse(response['data']);
                return response;
            });
    }
}
