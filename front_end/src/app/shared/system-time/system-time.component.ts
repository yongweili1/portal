import { Component, OnInit, Input } from '@angular/core';
import * as moment from 'moment';

import { SystemTimeService } from './system-time.service';

@Component({
  selector: 'mpt-system-time',
  template: `
    <div class="system-time"
    (mouseenter)="isShowFullTime = true"
    (mouseleave)="isShowFullTime = false"
    [innerHTML]="displayTime | mySanitizeHtml"></div>
  `
})

export class SystemTimeComponent implements OnInit {
  public displayTime: string = '';
  public isShowFullTime: boolean = false;
  @Input() public shortFormat: string = 'HH:mm:ss';
  @Input() public fullFormat: string = `YYYY-MM-DD HH:mm:ssZ`;

  private time: Date;

  public constructor(private systemTimeService: SystemTimeService) {

  }
  public ngOnInit() {
    // this.systemTimeService.getSystemTime().subscribe((time) => {
    //   this.time = time;
    //   this.isShowFullTime ? this.showFullTime() : this.showShortTime();
    // });
  }

  private showShortTime() {
    this.displayTime = moment(this.time).format(this.shortFormat);
  }

  private showFullTime() {
    this.displayTime = moment(this.time).format(this.fullFormat);
  }
}
