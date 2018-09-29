import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { Subject } from 'rxjs/Subject';
import { TranslateService } from '@ngx-translate/core';

export interface ToastEvent {
  message: string;
  level: string;
}

export interface Toast {
  message: string;
  level: string;
  styles: any[];
}

@Injectable()
export class ToastService {
  private toastEvents: Subject<ToastEvent> = new Subject<Toast>();

  constructor(private translateService: TranslateService) { }

  get events(): Observable<ToastEvent> {
    return this.toastEvents;
  }

  public success(message: string);
  public success(message: string, interpolateParams?: Object) {
    this.translate(message, interpolateParams, 'success');
  }

  public warning(message: string);
  public warning(message: string, interpolateParams?: Object) {
    this.translate(message, interpolateParams, 'warning');
  }

  public error(message: string);
  public error(message: string, interpolateParams?: Object) {
    this.translate(message, interpolateParams, 'error');
  }

  private publish(toast: ToastEvent) {
    this.toastEvents.next(toast);
  }

  private translate(message: string, interpolateParams?: Object, level: string = 'warning') {
    this.translateService.get(message, interpolateParams).subscribe((res) => {
      this.publish({ message: res, level });
    }, () => {
      this.publish({ message, level });
    });
  }

}
