import {
  Directive, ElementRef, Input, Output, Renderer, NgZone,
  HostListener, EventEmitter, OnChanges, OnDestroy, SimpleChange
} from '@angular/core';

import * as ECharts from 'echarts';

@Directive({
  selector: '[echarts]'
})
export class EchartsDirective implements OnChanges, OnDestroy {
  @Input() options: any;
  @Input() dataset: Array<any>;
  @Input() theme: string;
  @Input() loading: boolean;
  @Input() width: string;
  @Input() height: string;

  // chart events:
  @Output() chartInit: EventEmitter<any> = new EventEmitter<any>();
  @Output() chartClick: EventEmitter<any> = new EventEmitter<any>();
  @Output() chartDblClick: EventEmitter<any> = new EventEmitter<any>();
  @Output() chartMouseDown: EventEmitter<any> = new EventEmitter<any>();
  @Output() chartMouseUp: EventEmitter<any> = new EventEmitter<any>();
  @Output() chartMouseOver: EventEmitter<any> = new EventEmitter<any>();
  @Output() chartMouseOut: EventEmitter<any> = new EventEmitter<any>();
  @Output() chartGlobalOut: EventEmitter<any> = new EventEmitter<any>();
  @Output() chartContextMenu: EventEmitter<any> = new EventEmitter<any>();
  @Output() chartDataZoom: EventEmitter<any> = new EventEmitter<any>();

  private myChart: any = null;
  private currentWindowWidth = null;

  constructor(private el: ElementRef, private renderer: Renderer, private _ngZone: NgZone) {
  }

  private createChart() {
    this.theme = this.theme || 'default';
    this.width = this.width || this.options.width || 'auto';
    this.height = this.height || this.options.height || 'auto';
    this.options.width = this.width;
    this.options.height = this.height;
    this.currentWindowWidth = window.innerWidth;
    // return echarts.init(this.el.nativeElement);
    if (this.theme) {
      return this._ngZone.runOutsideAngular(() => {
        return ECharts.init(this.el.nativeElement, this.theme, this.options);
      });
    } else {
      return this._ngZone.runOutsideAngular(() => {
        return ECharts.init(this.el.nativeElement, this.options);
      });
    }
  }

  private updateChart() {
    this.myChart.setOption(this.options, true);
    this.myChart.resize();
  }

  private resetChart() {
    this.myChart.setOption({}, true);
  }

  @HostListener('window:resize', ['$event']) onWindowResize(event) {
    if (event.target.innerWidth !== this.currentWindowWidth) {
      this.currentWindowWidth = event.target.innerWidth;
      if (this.myChart) {
        this.myChart.resize();
      }
    }
  }

  ngOnChanges(changes: { [propertyName: string]: SimpleChange }) {
    if (changes['options']) {
      this.onOptionsChange(this.options);
    }

    if (changes['dataset']) {
      this.onDatasetChange(this.dataset);
    }

    if (changes['loading']) {
      this.onLoadingChange(this.loading);
    }
  }

  ngOnDestroy() {
    if (this.myChart) {
      this.myChart.dispose();
      this.myChart = null;
    }
  }

  onOptionsChange(opt: any) {
    if (opt) {
      if (!this.myChart) {
        this.myChart = this.createChart();
        // output echart instance:
        this.chartInit.emit(this.myChart);

        // register events:
        this.registerEvents(this.myChart);
      }

      if (this.hasData()) {
        this.updateChart();
      } else if (this.dataset && this.dataset.length) {
        this.mergeDataset(this.dataset);
        this.updateChart();
      } else if (JSON.stringify(opt) === '{}' && !this.dataset) {
        this.resetChart();
      }
    }
  }

  onDatasetChange(dataset: Array<any>) {
    if (this.myChart && this.options) {
      if (!this.options.series) {
        this.options.series = [];
      }

      this.mergeDataset(dataset);
      this.updateChart();
    }
  }

  onLoadingChange(loading: boolean) {
    if (this.myChart) {
      if (loading) {
        this.myChart.showLoading();
      } else {
        this.myChart.hideLoading();
      }
    }
  }

  private mergeDataset(dataset: any[]) {
    for (let i = 0, len = dataset.length; i < len; i++) {
      if (!this.options.series[i]) {
        this.options.series[i] = { data: dataset[i] };
      } else {
        this.options.series[i].data = dataset[i];
      }
    }
  }

  /**
   * method to check if the option has dataset.
   */
  hasData(): boolean {
    if (this.options.baseOption && this.options.baseOption.timeline) {
      const options = this.options.options;

      if (options.length) {
        for (const o of options) {
          if (o.series) {
            for (const serie of o.series) {
              if (serie.data && serie.data.length > 0) {
                return true;
              }
            }
          }
        }
      }

      return false;
    }

    if (!this.options.series || !this.options.series.length) {
      return false;
    }

    for (const serie of this.options.series) {
      if (serie.data && serie.data.length > 0) {
        return true;
      }
    }

    return false;
  }

  private registerEvents(myChart: any) {
    if (myChart) {
      // register mouse events:
      myChart.on('click', (e: any) => { this.chartClick.emit(e); });
      myChart.on('dblClick', (e: any) => { this.chartDblClick.emit(e); });
      myChart.on('mousedown', (e: any) => { this.chartMouseDown.emit(e); });
      myChart.on('mouseup', (e: any) => { this.chartMouseUp.emit(e); });
      myChart.on('mouseover', (e: any) => { this.chartMouseOver.emit(e); });
      myChart.on('mouseout', (e: any) => { this.chartMouseOut.emit(e); });
      myChart.on('globalout', (e: any) => { this.chartGlobalOut.emit(e); });
      myChart.on('contextmenu', (e: any) => { this.chartContextMenu.emit(e); });

      // other events;
      myChart.on('dataZoom', (e: any) => { this.chartDataZoom.emit(e); });
    }
  }

  public clear() {
    if (this.myChart) {
      this.myChart.clear();
    }
  }
}
