import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { AppConfigService } from '../../../../app.config';
import { ContourModel } from '../model/contour.model';


@Injectable()
export class ContourService {

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

    save(dto: ContourModel): Observable<any> {
        return this.http.post<ContourModel>(`${this.appConfig.apiUrl}/image/contour/`, dto, this.options)
            .map(response => response['body']);
    }

    delete(roi_uid, slice_index): Observable<any> {
        const getParams = new HttpParams()
            .set('roi_uid', roi_uid)
            .set('slice_index', slice_index);
        return this.http.delete(`${this.appConfig.apiUrl}/image/contour/`, { params: getParams });
    }

    getContours(roi_uids, slice_index): Observable<any> {
        const getParams = new HttpParams()
            .set('roi_uids', roi_uids)
            .set('slice_index', slice_index);
        return this.http.get(`${this.appConfig.apiUrl}/image/contour/`, { params: getParams });
    }
}
