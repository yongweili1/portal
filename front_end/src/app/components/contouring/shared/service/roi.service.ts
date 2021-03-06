import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/Observable';
import { AppConfigService } from '../../../../app.config';
import { RoiModel } from '../model/roi.model';


@Injectable()
export class RoiService {
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

    create(dto: RoiModel): Observable<any> {
        return this.http.post<any>(`${this.appConfig.apiUrl}/image/roi/`, dto, this.options)
            .map(response => response['body']);
    }

    get(seriesuid: string): Observable<any> {
        const getParams = new HttpParams()
            .set('seriesuid', seriesuid);
        return this.http.get<any>(`${this.appConfig.apiUrl}/image/roi/`, { params: getParams })
            .map(response => {
                const rois = new Array();
                response.data.forEach(roi => {
                    rois.push(new RoiModel(roi));
                });
                response.data = rois;
                return response;
            });
    }

    update(dto: RoiModel): Observable<any> {
        return this.http.put<any>(`${this.appConfig.apiUrl}/image/roi/`, dto, this.options)
            .map(x => x['body']);
    }

    delete(ids: any): Observable<any> {
        const getParams = new HttpParams()
            .set('ids', ids);
        return this.http.delete<any>(`${this.appConfig.apiUrl}/image/roi/`, { params: getParams });
    }

    startSeg(roiData): Observable<any> {
        return this.http.post<any>(`${this.appConfig.apiUrl}/algproxy/results/`, roiData, this.options)
            .map(x => x['body']);
    }

    contourToMask(seriesuid, roiuid): Observable<any> {
        const getParams = new HttpParams()
            .set('seriesuid', seriesuid)
            .set('roiuid', roiuid);
        return this.http.get(`${this.appConfig.apiUrl}/algproxy/results/`,
            { params: getParams, responseType: 'blob' });
    }
}
