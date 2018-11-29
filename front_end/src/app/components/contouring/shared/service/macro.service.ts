import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { AppConfigService } from '../../../../app.config';
import { Observable } from 'rxjs';


@Injectable()
export class MacroService {

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

    macro(status: any): Observable<any> {
        const getParams = new HttpParams()
            .set('macro_status', status);
        return this.http.get(`${this.appConfig.apiUrl}/macro/macro/`, { params: getParams });
    }
}
