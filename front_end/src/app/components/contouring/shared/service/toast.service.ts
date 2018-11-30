import { Injectable } from '@angular/core';
import { MessageService } from 'primeng/api';

@Injectable()

export class ToastService {

    constructor(private _svc: MessageService) {
    }

    info(msg: any) {
        this._svc.add({ severity: 'info', summary: 'Info Message', detail: msg });
    }

    success(msg: any) {
        this._svc.add({ severity: 'success', summary: 'Success Message', detail: msg });
    }

    warning(msg: any) {
        this._svc.add({ severity: 'warning', summary: 'Warning Message', detail: msg });
    }

    error(msg: any) {
        this._svc.add({ severity: 'error', summary: 'Error Message', detail: msg });
    }
}
