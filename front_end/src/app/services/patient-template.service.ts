import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { AppConfigService } from '../app.config';
import {
    PatientPageDeleteRequest,
    PatientPageRequest,
    PatientTemplateInfo
} from '../components/patient-template/shared/patient-template.model';
import { Page } from '../shared/models/pagination';


@Injectable()
export class PatientTemplateService {
    headers: HttpHeaders;
    constructor(
        private http: HttpClient,
        private appConfig: AppConfigService
    ) {
        this.headers = new HttpHeaders({
            'Content-Type': 'application/json',
            // 'withCredentials': 'true'
        });
    }

    public getPatientTemplate(pageRequest: PatientPageRequest): Observable<any> {
        const searchParams = new HttpParams()
            .set('size', pageRequest.size.toString())
            .set('page', pageRequest.page.toString())
            .set('patientId', pageRequest.patientId)
            .set('patientName', pageRequest.patientName)
            .set('gender', pageRequest.gender)
            .set('modality', pageRequest.modality)
            .set('studyDate', pageRequest.studyDate.toString());
        console.log(this.headers);
        return this.http.get<any>(`${this.appConfig.apiUrl}/patient/patinfolist/`,
                { headers: this.headers, params: searchParams });
    }

    public deletePatientTemplate(deleteRequest: PatientPageDeleteRequest): Observable<any> {
        const searchParams = new HttpParams()
            .set('size', deleteRequest.size.toString())
            .set('page', deleteRequest.page.toString())
            .set('patientId', deleteRequest.patientId)
            .set('studyId', deleteRequest.studyId)
            .set('seriesId', deleteRequest.seriesId);
        return this.http
            .delete<any>(`${this.appConfig.apiUrl}/patient/patinfolist/`, { headers: this.headers, params: searchParams });
    }
}
