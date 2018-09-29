import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';
import { SelectItem } from 'primeng/primeng';
import * as moment from 'moment';
import { CalendarLocales } from '../../shared/locales/calendar.locales';
import { DateRange } from './datetime-range-picker.model';
@Component({
  selector: 'datetime-range-picker',
  templateUrl: './datetime-range-picker.component.html',
  styleUrls: ['./datetime-range-picker.component.less']
})
export class DateTimeRangePickerComponent implements OnInit {
  @Input() public visible: boolean = false;
  @Input() public hideCallback: Function;
  @Input() public showTime: boolean = true;
  @Input() public showSeconds: boolean = true;

  @Output() public onDateRangeChanged = new EventEmitter();

  public selectedType = 0;

  public canlendarLocale: any;

  public dateRange: DateRange = {
    fromDate: new Date(),
    toDate: new Date(moment().format('YYYY-MM-DD 23:59:59'))
  };

  public regularDate: SelectItem[] = [];

  constructor(
    private calendarLocales: CalendarLocales
) { }

  public ngOnInit() {
    this.searchDate(0);
    this.regularDate.push({ label: '最近一天', value: 0 });
    this.regularDate.push({ label: '最近3天', value: 2 });
    this.regularDate.push({ label: '最近一周', value: 3 });
    this.regularDate.push({ label: '最近一月', value: 4 });
    this.regularDate.push({ label: '最近一年', value: 5 });

    this.canlendarLocale = this.calendarLocales.getCurrentLocale();
  }

  private show() {
    this.visible = true;
  }

  public hide() {
    this.visible = false;
  }

  public onHide() {
    if (this.hideCallback && typeof this.hideCallback === 'function') {
      this.hideCallback();
    }
  }

  public searchDate(s: number) {
    if (s === 3) {
      let fromDate = new Date(moment().subtract(1, 'weeks').format('YYYY-MM-DD 00:00:00'));
      this.creatDate(fromDate);
    } else if (s === 4) {
      let fromDate = new Date(moment().subtract(1, 'months').format('YYYY-MM-DD 00:00:00'));
      this.creatDate(fromDate);
    } else if (s === 5) {
      let fromDate = new Date(moment().subtract(1, 'years').format('YYYY-MM-DD 00:00:00'));
      this.creatDate(fromDate);
    } else {
      let fromDate = new Date(moment().subtract(s, 'days').format('YYYY-MM-DD 00:00:00'));
      this.creatDate(fromDate);
    }
  }

  private creatDate(fromDate: Date) {
    let toDate = new Date(moment().format('YYYY-MM-DD 23:59:59'));
    this.dateRange = {
      fromDate,
      toDate
    };
    this.onOK();
  }

  public onOK() {
    this.onDateRangeChanged.emit(this.dateRange);
    this.hide();
  }

}
