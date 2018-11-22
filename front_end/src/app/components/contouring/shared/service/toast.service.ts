import { Injectable } from '@angular/core';
import { MessageService } from 'primeng/api';

@Injectable()

export class ToastService {

    constructor(private _svc: MessageService) {
    }

    info(msg: any) {
        this._svc.add({ severity: 'info', detail: msg });
    }

    success(msg: any) {
        this._svc.add({ severity: 'success', detail: msg });
    }

    warning(msg: any) {
        this._svc.add({ severity: 'warning', detail: msg });
    }

    error(msg: any) {
        this._svc.add({ severity: 'error', detail: msg });
    }
}
