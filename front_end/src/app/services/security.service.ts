import { HttpClient, HttpHeaders, HttpResponse } from '@angular/common/http';
import { Observable } from 'rxjs/Observable';
import { Subject } from 'rxjs/Subject';

export class SecurityService {
    protected options: any;
    private headers: HttpHeaders;

    constructor() {
        // this.headers = new HttpHeaders({
        //     'Authorization': 'Bearer ' + abp.auth.getToken()
        // });
        this.headers = new HttpHeaders();
        this.headers.append('Content-Type', 'application/json');
        this.headers.append('Accept', 'application/json');

        this.options = {
            headers: this.headers,
            observe: "response",
        };
    }

    protected processStatus(response: HttpResponse<any>): Observable<any> {
        const status = response.status;
        
        if (status === 401) {
            return throwException("A server error occurred.", status, response.body, response.headers);
        } else if (status === 403) {
            return throwException("A server error occurred.", status, response.body, response.headers);
        } else if (status !== 200 && status !== 204) {
            return throwException("An unexpected server error occurred.", status, response.body, response.headers);
        }
    }
}

export class SwaggerException extends Error {
    message: string;
    status: number;
    response: string;
    headers: { [key: string]: any; };
    result: any;

    constructor(message: string, status: number, response: string, headers: { [key: string]: any; }, result: any) {
        super();

        this.message = message;
        this.status = status;
        this.response = response;
        this.headers = headers;
        this.result = result;
    }

    protected isSwaggerException = true;

    static isSwaggerException(obj: any): obj is SwaggerException {
        return obj.isSwaggerException === true;
    }
}

function throwException(message: string, status: number, response: string, headers: { [key: string]: any; }, result?: any): Observable<any> {
    if (result !== null && result !== undefined)
        return Observable.throw(result);
    else
        return Observable.throw(new SwaggerException(message, status, response, headers, null));
}
