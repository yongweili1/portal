import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { Observable, of } from 'rxjs';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { HttpParams } from "@angular/common/http";

import { AppConfigService } from '../app.config';
import { AuthHttp } from '../core/auth-http';
import { Page, PageRequest } from '../shared/models/pagination';
import {
    PatientPageRequest,
    PatientTemplateInfoRequest,
    PatientPageDeleteRequest
} from '../components/patient-template/shared/patient-template.model';
import { ToastService } from '../core/toast.service';
import { PacsModule } from '../components/pacs/pacs.module'

@Injectable({
    providedIn:PacsModule
})
export class PatientTemplateService {
    headers: HttpHeaders;
    constructor(
        private http: HttpClient,
        private appConfig: AppConfigService,
        private messageService: ToastService,
    ) {
        this.headers = new HttpHeaders({
            'Content-Type': 'application/json',
            // 'withCredentials': 'true'
        })
    }

    public getPatientTemplate(pageRequest: PatientPageRequest): Observable<Page<PatientPageRequest>> {
        const searchParams = new HttpParams()
            .set('size', pageRequest.size.toString())
            .set('page', pageRequest.page.toString())
            .set('patientId', pageRequest.patientId)
            .set('patientName', pageRequest.patientName)
            .set('gender', pageRequest.gender)
            .set('modality', pageRequest.modality)
            .set('studyDate', pageRequest.studyDate.toString());
        console.log(this.headers);
        return this.http
            .get<Page<PatientPageRequest>>(`${this.appConfig.apiUrl}/patientinformations/patinfolist/`, { headers: this.headers, params: searchParams });
    }

    public deletePatientTemplate(deleteRequest: PatientPageDeleteRequest): Observable<any> {
        const searchParams = new HttpParams()
            .set('size', deleteRequest.size.toString())
            .set('page', deleteRequest.page.toString())
            .set('patientId', deleteRequest.patientId)
        return this.http
            .delete<any>(`${this.appConfig.apiUrl}/patientinformations/patinfolist/`, { headers: this.headers, params: searchParams });
    }



}
const httpOptions = {
    headers: new HttpHeaders({ 'Content-Type': 'application/json' })
};

