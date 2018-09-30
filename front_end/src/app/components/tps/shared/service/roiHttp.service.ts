import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders, HttpResponse, HttpRequest } from '@angular/common/http';
import { SecurityService } from '../../../../services/security.service';
import { Observable } from 'rxjs/Observable';
import { StorageService } from './storage.service';

@Injectable()

export class RoiHttpService extends SecurityService {
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

    PostCreateRoiByAtlas(data: any) {
        const baseUrl = (this.storageService.retrieve("PATIENT_API_URLS") as string);
        let url = baseUrl + "services/app/Roi/CreateRoiByAtlasSeg";

        return this.http.post<any>(url, data, this.options).map((response: HttpResponse<any>) => {
            if (response.status == 200) {
                const _responseText = response.body.result
                let result200: any = null;
                result200 = _responseText ? RoiDto.fromJS(_responseText) : new RoiDto();

                return result200;
            }
            this.processStatus(response);
        });
    }
}

interface IRoiDto {
    roiGeometry: {};
    roiProperties: {};
}

class RoiDto implements IRoiDto {
    roiGeometry: {};
    roiProperties: {};

    constructor(data?: any) {
        if (data) {
            for (var property in data) {
                if (data.hasOwnProperty(property))
                    (<any>this)[property] = (<any>data)[property];
            }
        }
    }

    init(data?: any) {
        this.roiGeometry = data["roiGeometry"];
        this.roiProperties = data["roiProperties"];
    }

    static fromJS(data: any): RoiDto {
        let result = new RoiDto();
        result.init(data);
        return result;
    }
}
