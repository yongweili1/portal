import { Component, OnInit } from '@angular/core';
import { EventAggregator } from '../../../shared/common/event_aggregator';
import { LazyExcuteHandler } from '../lazy_excute_handler';

@Component({
  selector: 'mpt-slider-bar',
  templateUrl: './slider-bar.component.html',
  styleUrls: ['./slider-bar.component.less']
})
export class SliderBarComponent implements OnInit {
  progress: number;

  totalSliceCount: number;
  currentSliceNum: number;

  lazyExcuteHandler: LazyExcuteHandler;

  constructor() {
    this.totalSliceCount = 0;
    this.currentSliceNum = 0;
    this.progress = 100;
    this.lazyExcuteHandler = new LazyExcuteHandler()

    EventAggregator.Instance().volumnSize.subscribe(value=>{
      this.totalSliceCount = value[2]
    });

    EventAggregator.Instance().sliceIndex.subscribe(value=>{
      this.currentSliceNum = value;
      this.progress = this.currentSliceNum / this.totalSliceCount * 100;
      console.log('Current progress', this.progress)
    })
  }

  ngOnInit() {
  }

  // Callback to invoke on value change via slide.
  handleChange(e) {
    console.log('handleChange', e.value)
    let newIndex = e.value / 100 * this.totalSliceCount
    let pageDelta = Math.floor(newIndex - this.currentSliceNum);
    this.currentSliceNum = newIndex;
    EventAggregator.Instance().pageDelta.publish(pageDelta);
  }

  // Callback to invoke when slide stops.
  handleSlideEnd(e) {
    console.log('handleSlideEnd', e.value)

  }

}
