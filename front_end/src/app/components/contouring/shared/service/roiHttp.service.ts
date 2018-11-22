import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/Observable';
import { AppConfigService } from '../../../../app.config';
import { RoiModel } from '../model/roi.model';


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

    PostCreateNewROI(roiData: RoiModel): Observable<any> {
        let data = { ROIId: roiData.ROIId, ROIName: roiData.ROIName, ROIColor: roiData.ROIColor };
        return this.http.post<any>(`${this.appConfig.apiUrl}/roi/roidata/`, data, this.options);
    }

    GetROIConfig(seriesid: any): Observable<any> {
        const getParams = new HttpParams()
            .set('seriesuid', seriesid);
        return this.http.get<any>(`${this.appConfig.apiUrl}/roi/roidata/`, { params: getParams });
    }

    UpdateROIConfig(roiData: RoiModel): Observable<any> {
        let data = { ROIId: roiData.ROIId, ROIName: roiData.ROIName, ROIColor: roiData.ROIColor };
        return this.http.put<any>(`${this.appConfig.apiUrl}/roi/roidata/`, data, this.options);
    }

    DeleteROIConfig(roiId: any): Observable<any> {
        const getParams = new HttpParams()
            .set('ROIId', roiId);
        return this.http.delete<any>(`${this.appConfig.apiUrl}/roi/roidata/`, { params: getParams });
    }

    CreateNewSegROI(roiData): Observable<any> {
        return this.http.post<any>(`${this.appConfig.apiUrl}/algproxy/results/`, roiData, this.options);
    }
}
