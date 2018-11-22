import { HttpClient, HttpResponse } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/Observable';
import { SecurityService } from '../../../../services/security.service';
import { StorageService } from './storage.service';


@Injectable()

export class PatientHttpService extends SecurityService {
    private http: HttpClient;
    private storageService: StorageService;

    constructor(
        private _http: HttpClient,
        private _storageService: StorageService
    ) {
        super();
        this.http = _http;
        this.storageService = _storageService;
    }

    GetPatientData(): Observable<HttpResponse<any>> {
        const baseUrl = (this.storageService.retrieve("PATIENT_API_URLS") as string);
        let url = baseUrl + "services/app/Patient/GetAll";

        return this.http.get<any>(url, this.options).map((response: HttpResponse<any>) => {
            if (response.status == 200) {
                const _responseText = response.body.result;
                let result200: any = null;
                // let resultData200 = _responseText === "" ? null : JSON.parse(_responseText);
                result200 = _responseText ? PatientArrayDto.fromJS(_responseText) : new PatientArrayDto();

                return result200;
            }
            this.processStatus(response);
        });
    }

    GetPatientById(id: number): Observable<HttpResponse<any>> {
        const baseUrl = (this.storageService.retrieve("PATIENT_API_URLS") as string);
        let url = baseUrl + "services/app/Patient/Get?patientId=" + id;
        return this.http.get<any>(url, this.options).map((response: HttpResponse<any>) => {
            if (response.status == 200) {
                const _responseText = response.body.result;
                let result200: any = null;
                result200 = _responseText ? PatientDto.fromJS(_responseText) : new PatientDto();

                return result200;
            }
            this.processStatus(response);
        });
    }

    autoSegmentation(id: number): Observable<HttpResponse<any>> {
        const baseUrl = (this.storageService.retrieve("PATIENT_API_URLS") as string);
        let url = baseUrl + "services/app/Patient/GetSitesByAutoSegmentationBasedOn?autoSegmentationBasedOnValue=" + id;
        return this.http.get<any>(url, this.options).map((response: HttpResponse<any>) => {
            if (response.status == 200) {
                const _responseText = response.body.result;
                let result200: any = null;
                result200 = _responseText ? organTree.fromJS(_responseText) : new organTree();

                return result200;
            }
            this.processStatus(response);
        });
    }
}

interface IorganTree {
    sites: any;
}

class organTree implements IorganTree {
    sites: any;

    constructor(data?: any) {
        if (data) {
            for (var property in data) {
                if (data.hasOwnProperty(property))
                    (<any>this)[property] = (<any>data)[property];
            }
        }
    }

    init(data?: any) {
        this.sites = data["sites"];
    }

    static fromJS(data: any): organTree {
        let result = new organTree();
        result.init(data);
        return result.sites;
    }
}

interface IPatientDto {
    id: number;
    patientName: string;
    patientID: string;
    patientsBirthDate: string;
    patientSex: string;
    series: SeriesDto[];
}

class PatientDto implements IPatientDto {
    id: number;
    patientName: string;
    patientID: string;
    patientsBirthDate: string;
    patientSex: string;
    series: SeriesDto[];

    constructor(data?: IPatientDto) {
        if (data) {
            for (var property in data) {
                if (data.hasOwnProperty(property))
                    (<any>this)[property] = (<any>data)[property];
            }
        }
    }

    init(data?: any) {
        if (data) {
            this.id = data["id"];
            this.patientName = data["patientName"];
            this.patientID = data["patientID"];
            this.patientsBirthDate = data["patientsBirthDate"];
            this.patientSex = data["patientSex"];

            if (data["series"]["items"].length != 0) {
                this.series = [];
                for (let item of data["series"]["items"]) {
                    this.series.push(SeriesDto.fromJS(item));
                }
            }
        }
    }

    static fromJS(data: any): PatientDto {
        let result = new PatientDto();
        result.init(data);
        return result;
    }
}

interface IPatientArrayDto {
    items: PatientDto[];
}

class PatientArrayDto implements IPatientArrayDto {
    items: PatientDto[];

    constructor(data?: IPatientArrayDto) {
        if (data) {
            for (var property in data) {
                if (data.hasOwnProperty(property))
                    (<any>this)[property] = (<any>data)[property];
            }
        }
    }

    init(data?: any) {
        if (data) {
            if (data["items"] && data["items"].constructor == Array) {
                this.items = [];
                for (let item of data["items"]) {
                    this.items.push(PatientDto.fromJS(item));
                }
            }
        }
    }

    static fromJS(data: any): PatientArrayDto {
        let result = new PatientArrayDto();
        result.init(data);
        return result;
    }
}

interface ISeriesDto {
    seriesName: string;
    seriesUId: string;
    bytesPerPixel: number;
    rows: number;
    columns: number;
    firstImagePosition: number[];
    lastImagePosition: number[];
    imageOrientation: number[];
    pixelRepresentation: number;
    pixelSpacing: number[];
    rescaleSlope: number;
    rescaleIntercept: number;
    sliceCounts: number;
    windowingLevel: number;
    windowingWidth: number;
    id: number;
}

class SeriesDto implements ISeriesDto {
    seriesName: string;
    seriesUId: string;
    bytesPerPixel: number;
    rows: number;
    columns: number;
    firstImagePosition: number[];
    lastImagePosition: number[];
    imageOrientation: number[];
    pixelRepresentation: number;
    pixelSpacing: number[];
    rescaleSlope: number;
    rescaleIntercept: number;
    sliceCounts: number;
    windowingLevel: number;
    windowingWidth: number;
    id: number;

    constructor(data?: ISeriesDto) {
        if (data) {
            for (var property in data) {
                if (data.hasOwnProperty(property))
                    (<any>this)[property] = (<any>data)[property];
            }
        }
    }

    init(data?: any) {
        if (data) {
            this.seriesName = data["seriesName"];
            this.seriesUId = data["seriesUId"];
            this.bytesPerPixel = data["bytesPerPixel"];
            this.rows = data["rows"];
            this.columns = data["columns"];
            if (data["firstImagePosition"] && data["firstImagePosition"].constructor == Array) {
                this.firstImagePosition = [];
                for (let item of data["firstImagePosition"]) {
                    this.firstImagePosition.push(item);
                }
            }
            if (data["lastImagePosition"] && data["lastImagePosition"].constructor == Array) {
                this.lastImagePosition = [];
                for (let item of data["lastImagePosition"]) {
                    this.lastImagePosition.push(item);
                }
            }
            if (data["imageOrientation"] && data["imageOrientation"].constructor == Array) {
                this.imageOrientation = [];
                for (let item of data["imageOrientation"]) {
                    this.imageOrientation.push(item);
                }
            }
            this.pixelRepresentation = data["pixelRepresentation"];
            if (data["pixelSpacing"] && data["pixelSpacing"].constructor == Array) {
                this.pixelSpacing = [];
                for (let item of data["pixelSpacing"]) {
                    this.pixelSpacing.push(item);
                }
            }
            this.rescaleSlope = data["rescaleSlope"];
            this.rescaleIntercept = data["rescaleIntercept"];
            this.sliceCounts = data["sliceCounts"];
            this.windowingLevel = data["windowingLevel"];
            this.windowingWidth = data["windowingWidth"];
            this.id = data["id"];
        }
    }

    static fromJS(data: any): SeriesDto {
        let result = new SeriesDto();
        result.init(data);
        return result;
    }
}
