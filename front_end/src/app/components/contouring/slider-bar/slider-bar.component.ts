import { Component, OnInit } from '@angular/core';
import { EventAggregator } from '../../../shared/common/event_aggregator';

@Component({
    selector: 'mpt-slider-bar',
    templateUrl: './slider-bar.component.html',
    styleUrls: ['./slider-bar.component.less']
})
export class SliderBarComponent implements OnInit {
    progress: number;

    totalSliceCount: number;
    currentSliceNum: number;

    constructor() {
        this.totalSliceCount = 0;
        this.currentSliceNum = 0;
        this.progress = 100;

        EventAggregator.Instance().volumnSize.subscribe(value => {
            this.totalSliceCount = value[2];
        });

        EventAggregator.Instance().sliceIndex.subscribe(value => {
            this.currentSliceNum = value;
            this.progress = this.currentSliceNum / this.totalSliceCount * 100;
            console.log('Current progress', this.progress);
        });
    }

    ngOnInit() {
    }

    // Callback to invoke on value change via slide.
    handleChange(e) {
        console.log('handleChange', e.value);
        const newIndex = e.value / 100 * this.totalSliceCount;
        const pageDelta = Math.floor(newIndex - this.currentSliceNum);
        this.currentSliceNum = newIndex;
        EventAggregator.Instance().pageDelta.publish(pageDelta);
    }

    // Callback to invoke when slide stops.
    handleSlideEnd(e) {
        console.log('handleSlideEnd', e.value);
    }
}
