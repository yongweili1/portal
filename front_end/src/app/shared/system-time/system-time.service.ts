import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';

@Injectable()
export class SystemTimeService {
  private interval: number = 1000;

  //  public getSystemTime(): Observable<Date> {
  //    //return Observable.interval(this.interval).map(() => { return new Date(); });
  //    return Observable.create().interval(this.interval).map(() => { return new Date(); });
  //  }
}
