import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { AppConfigService } from '../app.config';
import { EachPageRequest, PacsDownloadRequest, PacsPageRequest } from '../components/pacs/shared/pacs.model';
import { Page } from '../shared/models/pagination';


@Injectable()
export class PacsService {
    headers: HttpHeaders;

    constructor(
        private http: HttpClient,
        private appConfig: AppConfigService
    ) {
        this.headers = new HttpHeaders({
            'Content-Type': 'application/json',
            'withCredentials': 'true'
        })
    }

    public getPacs(pageRequest: PacsPageRequest): Observable<Page<PacsPageRequest>> {
        const searchParams = new HttpParams()
            .set('size', pageRequest.size.toString())
            .set('page', pageRequest.page.toString())
            .set('patientId', pageRequest.patientId)
            .set('patientName', pageRequest.patientName)
            .set('patientAge', pageRequest.patientAge)
            .set('gender', pageRequest.gender)
            .set('modality', pageRequest.modality);
        console.log(this.headers);
        return this.http
            .get<Page<PacsPageRequest>>(`${this.appConfig.apiUrl}/pacs/pacspatient/`, { headers: this.headers, params: searchParams });
    }

    public getPage(pageRequest: EachPageRequest): Observable<Page<PacsPageRequest>> {
        const searchParams = new HttpParams()
            .set('size', pageRequest.size.toString())
            .set('page', pageRequest.page.toString())
        console.log(this.headers);
        return this.http
            .get<Page<PacsPageRequest>>(`${this.appConfig.apiUrl}/pacs/pacspage/`, { headers: this.headers, params: searchParams });
    }

    public getDownloadPacs(downloadRequest: PacsDownloadRequest): Observable<any> {
        const searchParams = new HttpParams()
            .set('patientId', downloadRequest.patientId);
        console.log(this.headers);
        return this.http
            .get<Page<PacsPageRequest>>(`${this.appConfig.apiUrl}/pacs/pacsseries/`, { headers: this.headers, params: searchParams });
    }
}
