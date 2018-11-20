import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders, HttpResponse, HttpRequest, HttpParams } from '@angular/common/http';
import { SecurityService } from '../../../../services/security.service';
import { Observable } from 'rxjs/Observable';
import { StorageService } from './storage.service';
import { AppConfigService } from '../../../../app.config';


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

    // PostCreateRoiByAtlas(data: any) {
    //     const baseUrl = (this.storageService.retrieve("PATIENT_API_URLS") as string);
    //     let url = baseUrl + "services/app/Roi/CreateRoiByAtlasSeg";

    //     return this.http.post<any>(url, data, this.options).map((response: HttpResponse<any>) => {
    //         if (response.status == 200) {
    //             const _responseText = response.body.result
    //             let result200: any = null;
    //             result200 = _responseText ? RoiDto.fromJS(_responseText) : new RoiDto();

    //             return result200;
    //         }
    //         this.processStatus(response);
    //     });
    // }

    PostCreateNewROI(roiData: any): Observable<any> {
        return this.http.post<any>(`${this.appConfig.apiUrl}/roi/roidata/`, roiData, this.options);
    }

    GetROIConfig(seriesid: any): Observable<any> {
        const getParams = new HttpParams()
            .set('seriesuid', seriesid);
        return this.http.get<any>(`${this.appConfig.apiUrl}/roi/roidata/`, { params: getParams });
    }

    UpdateROIConfig(roiData: any): Observable<any> {
        return this.http.put<any>(`${this.appConfig.apiUrl}/roi/roidata/`, roiData, this.options);
    }

    DeleteROIConfig(roiId: any): Observable<any> {
        const getParams = new HttpParams()
            .set('ROIId', roiId);
        return this.http.delete<any>(`${this.appConfig.apiUrl}/roi/roidata/`, { params: getParams });
    }

    CreateNewSegROI(roiData: any): Observable<any> {
        return this.http.post<any>(`${this.appConfig.apiUrl}/algproxy/results/`, roiData, this.options);
    }
}

// interface IRoiDto {
//     roiGeometry: {};
//     roiProperties: {};
// }

// class RoiDto implements IRoiDto {
//     roiGeometry: {};
//     roiProperties: {};

//     constructor(data?: any) {
//         if (data) {
//             for (var property in data) {
//                 if (data.hasOwnProperty(property))
//                     (<any>this)[property] = (<any>data)[property];
//             }
//         }
//     }

//     init(data?: any) {
//         this.roiGeometry = data["roiGeometry"];
//         this.roiProperties = data["roiProperties"];
//     }

//     static fromJS(data: any): RoiDto {
//         let result = new RoiDto();
//         result.init(data);
//         return result;
//     }
// }
