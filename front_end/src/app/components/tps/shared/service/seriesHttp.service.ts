import { Injectable } from '@angular/core';
import {  HttpClient,HttpHeaders } from '@angular/common/http';
import { SecurityService } from '../../../../services/security.service';
import { Observable } from 'rxjs/Observable';
import { StorageService } from './storage.service';
import {HttpParams} from "@angular/common/http";
import {LoadSeriesServiceMock} from '../../../../mocks/load-series-service.mock'

@Injectable()
export class SeriesHttpService {
    headers: HttpHeaders; 
    constructor(
        private http: HttpClient,
        private loadSeriesServiceMock:LoadSeriesServiceMock
    ) {
        const headers: HttpHeaders = new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded; charset=UTF-8' })
    }
/**
 * 从后端获取序列图片
 * @param seriesId 
 */
    GetSeriesPic(seriesId:any):Observable<string>{
        seriesId = '1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373';
        const getParams = new HttpParams()
        .set('seriesuid', seriesId.toString());
        return this.http.get<any>('http://localhost:8090/api/load-series',{params : getParams});
        //return this.http.get<string>('http://localhost:8000/image/volumes',{params : getParams});
        //return this.http.get<string>('http://10.9.19.139:8000/image/volumes/',{params : getParams});
        
    }

/**
 * 测试接口
 */
    testService():Observable<string>{
        //return this.http.get<any>('http://localhost:8090/api/load-series',{params : getParams});
        return this.http.get<string>('http://localhost:8000/image/sayhi');
        //return this.http.get<string>('http://10.9.19.139:8000/image/sayhi');
        
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
//     Axial: ACSDto;
//     Coronal: ACSDto;
//     Sagittal: ACSDto;
// }

// class MprSliceDetails implements IMprSliceDetails {
//     Axial: ACSDto;
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
//             this.Axial = ACSDto.fromJS(data["Axial"]);
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



