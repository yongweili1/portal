import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/Observable';
import { AppConfigService } from '../../../../app.config';
import { RoiModel } from '../model/roi.model';
import { RoiDto } from '../dto/roi.dto';


@Injectable()

export class RoiHttpService {
    headers: HttpHeaders
    options: any

    constructor(
        private http: HttpClient,
        private appConfig: AppConfigService,
    ) {
        this.headers = new HttpHeaders();
        this.headers.append('Content-Type', 'application/json');
        this.headers.append('Accept', 'application/json');

        this.options = {
            headers: this.headers,
            observe: "response",
        }
    }

    create(dto: RoiDto): Observable<any> {
        return this.http.post<any>(`${this.appConfig.apiUrl}/roi/roidata/`, dto, this.options);
    }

    get(seriesuid: string): Observable<any> {
        const getParams = new HttpParams()
            .set('seriesuid', seriesuid);
        return this.http.get<any>(`${this.appConfig.apiUrl}/roi/roidata/`, { params: getParams });
    }

    update(dto: RoiDto): Observable<any> {
        return this.http.put<any>(`${this.appConfig.apiUrl}/roi/roidata/`, dto, this.options);
    }

    delete(ids: any): Observable<any> {
        const getParams = new HttpParams()
            .set('ids', ids);
        return this.http.delete<any>(`${this.appConfig.apiUrl}/roi/roidata/`, { params: getParams });
    }

    CreateNewSegROI(roiData): Observable<any> {
        return this.http.post<any>(`${this.appConfig.apiUrl}/algproxy/results/`, roiData, this.options);
    }
}
