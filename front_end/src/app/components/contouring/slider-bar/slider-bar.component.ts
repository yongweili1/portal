import { Component, OnInit, Input, OnDestroy, Output, EventEmitter } from '@angular/core';
import { EventAggregator } from '../../../shared/common/event_aggregator';
import { Subscription } from 'rxjs';

@Component({
    selector: 'mpt-slider-bar',
    templateUrl: './slider-bar.component.html',
    styleUrls: ['./slider-bar.component.less']
})
export class SliderBarComponent implements OnInit, OnDestroy {
    progress: number;

    totalSliceCount: number;
    currentSliceNum: number;
    perSliceCount: number;
    currentSliceValue: number;

    volumnSizeSubscriber: Subscription;
    sliceIndexSubscriber: Subscription;

    @Input() tag: string;
    @Output() slideChangeEnd: EventEmitter<any> = new EventEmitter<any>();

    constructor() {
        this.totalSliceCount = 0;
        this.currentSliceNum = 0;
        this.progress = 100;
        this.perSliceCount = 0;
        this.currentSliceValue = 0;

        this.volumnSizeSubscriber = EventAggregator.Instance().volumnSize.subscribe(value => {
            this.totalSliceCount = value[2];
            this.perSliceCount = 100.00 / this.totalSliceCount;
        });

        this.sliceIndexSubscriber = EventAggregator.Instance().sliceIndex.subscribe(value => {
            this.currentSliceNum = value;
            if (value !== Math.floor(this.currentSliceValue / this.perSliceCount)) {
                this.progress = this.currentSliceNum / this.totalSliceCount * 100;
            }
        });
    }

    ngOnInit() {
    }

    ngOnDestroy() {
        this.volumnSizeSubscriber.unsubscribe();
        this.sliceIndexSubscriber.unsubscribe();
    }

    // Callback to invoke on value change via slide.
    handleChange(e) {
        console.log('handleChange', e.value);
        this.currentSliceValue = e.value;
        const newIndex = e.value / 100 * this.totalSliceCount;
        const pageDelta = Math.floor(newIndex - this.currentSliceNum);
        if (pageDelta < 1 && pageDelta > -1) {
            return;
        }
        // this.currentSliceNum = Math.floor(newIndex);
        EventAggregator.Instance().pageDelta.publish([this.tag, pageDelta]);
    }

    // Callback to invoke when slide stops.
    handleSlideEnd(e) {
        console.log('handleSlideEnd', e.value);
        this.currentSliceValue = e.value;
        const newIndex = e.value / 100 * this.totalSliceCount;
        const pageDelta = Math.floor(newIndex - this.currentSliceNum);
        this.slideChangeEnd.emit(pageDelta);
    }
}
