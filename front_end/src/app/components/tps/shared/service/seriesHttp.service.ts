import { Injectable } from '@angular/core';
import {  HttpClient,HttpHeaders } from '@angular/common/http';
import { AuthHttp } from '../../../../core/auth-http';
import { SecurityService } from '../../../../services/security.service';
import { Observable } from 'rxjs/Observable';
import { StorageService } from './storage.service';
import {HttpParams} from "@angular/common/http";
import {LoadSeriesServiceMock} from '../../../../mocks/load-series-service.mock'
import { ajax } from "rxjs/ajax";
import { AjaxService } from "../../../../services/ajax.service";

@Injectable()
export class SeriesHttpService {
    headers: HttpHeaders; 
    constructor(
        private aj:AjaxService,
        private http: HttpClient,
        private loadSeriesServiceMock:LoadSeriesServiceMock
    ) {
        const headers: HttpHeaders = new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded; charset=UTF-8' })
    }

getSeriesList(patientId:any):Observable<Array<any>>{
    const getParams = new HttpParams()
    .set('patineId', patientId);
    return this.http.get<Array<any>>('http://10.9.19.148:8000/image/serids/',{params : getParams});
}



/**
 * 从后端获取序列图片
 * @param seriesId 
 */
LoadSeries(seriesId:any):Observable<any>{
    const getParams = new HttpParams()
    .set('seriesuid', seriesId);
    return this.http.get('http://v:8000/image/volumes/',{params : getParams});
        
}

GetSeries(seriesId:any,focus:any,display:any,width:any,height:any):Observable<any>{
    const getParams = new HttpParams()
     .set('focus_view',focus)
     .set('seriesuid',seriesId)
     .set('width', width)
     .set('height', height)
     .set('display_view', display);
    return this.http.get('http://10.9.19.139:8000/image/images/',{params : getParams});
    
}

GetSeriesPic(focus:any,display:any,delta:any,width:any,height:any):Observable<any>{
    const getParams = new HttpParams()
     .set('delta', delta)
     .set('width', width)
     .set('height', height)
     .set('focus_view', focus)
     .set('display_view', display);
    return this.http.get('http://10.9.19.139:8000/image/pages/',{params : getParams});
    
}


    // PostSeriesLoad(id: number) {
    //     const baseUrl = (this.storageService.retrieve("PATIENT_API_URLS") as string);
    //     let url = baseUrl + "services/app/Series/Load?seriesId=" + id;

    //     return this.http.post<any>(url, null,this.options).map((response: HttpResponse<any>) => {
    //         if (response.status == 200) {
    //             return response;
    //         }
    //         this.processStatus(response);
    //     });
    // }

    // PostMpr(data: any): Observable<HttpResponse<any>> {
    //     const baseUrl = (this.storageService.retrieve("PATIENT_API_URLS") as string);
    //     let url = baseUrl + "services/app/Series/GetMpr";

    //     return this.http.post<any>(url, data, this.options).map((response: HttpResponse<any>) => {
    //         if (response.status == 200) {
    //             const _responseText = response.body.result
    //             let result200: any = null;
    //             result200 = _responseText ? MprDto.fromJS(_responseText) : new MprDto();

    //             return result200;
    //         }
    //         this.processStatus(response);
    //     });
    // }

    // PostMprJpeg(data: any) {
    //     const baseUrl = (this.storageService.retrieve("PATIENT_API_URLS") as string);
    //     let url = baseUrl + "services/app/Series/GetMprJpeg";

    //     return this.http.post<any>(url, data, this.options).map((response: HttpResponse<any>) => {
    //         if (response.status == 200) {
    //             const _responseText = response.body.result
    //             let result200: any = null;
    //             result200 = _responseText ? MprJpeg.fromJS(_responseText) : new MprJpeg();

    //             return result200;
    //         }
    //         this.processStatus(response);
    //     });
    // }

    // PostMprRawBuffer(data: any) {
    //     const baseUrl = (this.storageService.retrieve("PATIENT_API_URLS") as string);
    //     let url = baseUrl + "services/app/Series/GetMprRawBuffer";

    //     return this.http.post<any>(url, data, this.options).map((response: HttpResponse<any>) => {
    //         if (response.status == 200) {
    //             const _responseText = response.body.result
    //             let result200: any = null;
    //             result200 = _responseText ? MprRawBuffer.fromJS(_responseText) : new MprRawBuffer();

    //             return result200;
    //         }
    //         this.processStatus(response);
    //     });
    // }

}

// interface IMprDto {
//     seriesUid: string;
//     errorCode: number;
//     mprSliceDetails: MprSliceDetails;
// }

// class MprDto implements IMprDto{
//     seriesUid: string;
//     errorCode: number;
//     mprSliceDetails: MprSliceDetails;

//     constructor(data?: any) {
//         if (data) {
//             for (var property in data) {
//                 if (data.hasOwnProperty(property))
//                     (<any>this)[property] = (<any>data)[property];
//             }
//         }
//     }

//     init(data?: any) {
//         if (data) {
//             this.seriesUid = data["seriesUid"];
//             this.errorCode = data["errorCode"];
//             this.mprSliceDetails = MprSliceDetails.fromJS(data["mprSliceDetails"]);
//         }
//     }

//     static fromJS(data: any): MprDto {
//         let result = new MprDto();
//         result.init(data);
//         return result;
//     }
// }

// interface IMprSliceDetails {
//     Transverse: ACSDto;
//     Coronal: ACSDto;
//     Sagittal: ACSDto;
// }

// class MprSliceDetails implements IMprSliceDetails {
//     Transverse: ACSDto;
//     Coronal: ACSDto;
//     Sagittal: ACSDto;

//     constructor(data?: any) {
//         if (data) {
//             for (var property in data) {
//                 if (data.hasOwnProperty(property))
//                     (<any>this)[property] = (<any>data)[property];
//             }
//         }
//     }

//     init(data?: any) {
//         if (data) {
//             this.Transverse = ACSDto.fromJS(data["Transverse"]);
//             this.Coronal = ACSDto.fromJS(data["Coronal"]);
//             this.Sagittal = ACSDto.fromJS(data["Sagittal"]);
//         }
//     }

//     static fromJS(data: any): MprSliceDetails {
//         let result = new MprSliceDetails();
//         result.init(data);
//         return result;
//     }
// }

// interface IACSDto {
//     width: number;
//     height: number;
//     bytesPerPixel: number;
//     buffer: string;
// }

// class ACSDto implements IACSDto {
//     width: number;
//     height: number;
//     bytesPerPixel: number;
//     buffer: string;

//     constructor(data?: IACSDto) {
//         if (data) {
//             for (var property in data) {
//                 if (data.hasOwnProperty(property))
//                     (<any>this)[property] = (<any>data)[property];
//             }
//         }
//     }

//     init(data?: any) {
//         if (data) {
//             this.width = data["width"];
//             this.height = data["height"];
//             this.bytesPerPixel = data["bytesPerPixel"];
//             this.buffer = data["buffer"];
//         }
//     }

//     static fromJS(data: any): ACSDto {
//         let result = new ACSDto();
//         result.init(data);
//         return result;
//     }
// }

// interface IMprJpeg {
//     errorCode: number;
//     buffer: string;
// }

// class MprJpeg implements IMprJpeg{
//     errorCode: number;
//     buffer: string;

//     constructor(data?: any) {
//         if (data) {
//             for (var property in data) {
//                 if (data.hasOwnProperty(property))
//                     (<any>this)[property] = (<any>data)[property];
//             }
//         }
//     }

//     init(data?: any) {
//         if (data) {
//             this.errorCode = data["errorCode"];
//             this.buffer = data["buffer"];
//         }
//     }

//     static fromJS(data: any): MprJpeg {
//         let result = new MprJpeg();
//         result.init(data);
//         return result;
//     }
// }

// interface IMprRawBuffer {
//     seriesUid: string;
//     errorCode: number;
//     mprSliceDetails: MprSliceDetails;
// }

// class MprRawBuffer implements IMprRawBuffer {
//     seriesUid: string;
//     errorCode: number;
//     mprSliceDetails: MprSliceDetails;

//     constructor(data?: any) {
//         if (data) {
//             for (var property in data) {
//                 if (data.hasOwnProperty(property))
//                     (<any>this)[property] = (<any>data)[property];
//             }
//         }
//     }

//     init(data?: any) {
//         this.seriesUid = data["seriesUid"];
//         this.errorCode = data["errorCode"];
//         this.mprSliceDetails = MprSliceDetails.fromJS(data["mprSliceDetails"]);
//     }

//     static fromJS(data: any): MprRawBuffer {
//         let result = new MprRawBuffer();
//         result.init(data);
//         return result;
//     }
// }



