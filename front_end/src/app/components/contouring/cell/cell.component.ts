import { Component, EventEmitter, Input, OnChanges, Output, SimpleChanges } from '@angular/core';
import { EventAggregator } from '../../../shared/common/event_aggregator';
import { KeyValuePair } from '../../../shared/common/keyvaluepair';
import { CrosslineContainer } from '../shared/container/crossline_container';
import { ConMessageService } from '../shared/service/conMessage.service';
import { Point } from '../shared/tools/point';
declare var $: any;
declare var actions: any;

@Component({
    selector: 'mpt-cell',
    templateUrl: './cell.component.html',
    styleUrls: ['./cell.component.less']
})
export class CellComponent {
    id: string;
    container: any; imageCanvas: any; crossCanvas: any; actionCanvas: any; overlayCanvas: any;

    @Input() tag: any;

    @Output() onLocate: EventEmitter<any> = new EventEmitter<any>();
    @Output() onScroll: EventEmitter<any> = new EventEmitter<any>();
    @Output() onZoom: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onPan: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onRotate: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onChangeWwwl: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();

    imageData: any;
    crossPosition: Point;
    graphics: any;
    windowLevel: any = 0;
    windowWidth: any = 2000;
    actionInfo: any;

    constructor() {
        this.id = 'cell';
        this.crossPosition = new Point(0, 0);
        this.actionInfo = new KeyValuePair(actions.locate);
        EventAggregator.Instance().actionInfo.subscribe(value => {
            console.log('cell.component.ts get action info:', value.key())
            this.actionInfo = value;
        });

        EventAggregator.Instance().pageDelta.subscribe(value => {
            this.page(value);
        });

        EventAggregator.Instance().crossPoint.subscribe(value => {
            if (value[0] !== this.tag) {
                return;
            }
            const p = new Point(value[1].x, value[1].y);
            if (this.crossPosition.equals(p)) {
                return;
            }
            this.onLocate.emit([value[1].x, value[1].y]);
            this.crossPosition = p;
        });

        EventAggregator.Instance().eventData.subscribe(value => {
            if (value === undefined || value.length != 2) {
                return;
            }
            const action = value[0];
            const data = value[1];
            switch (action) {
                case actions.locate:
                    break;
                case actions.pan:
                    this.onPan.emit(data);
                    break;
                case actions.zoom:
                    this.onZoom.emit(data);
                    break;
                case actions.rotate:
                    this.onRotate.emit(data);
                    break;
                case actions.window:
                    const level = Math.round(this.windowLevel + this.windowWidth * data[1]);
                    const width = Math.round(this.windowWidth * data[0]);
                    if (level == this.windowLevel && width == this.windowWidth) {
                        return;
                    }
                    this.onChangeWwwl.emit([width, level]);
                    break;
                default:
                    break;
            }
        });
    }

    ngAfterViewInit() {
        this.container = $('#' + this.id + '-container').get(0);
        this.imageCanvas = $('#' + this.id + '-image').get(0);
        this.crossCanvas = $('#' + this.id + '-cross').get(0);
        this.overlayCanvas = $('#' + this.id + '-overlay').get(0);
        this.actionCanvas = $('#' + this.id + '-action').get(0);

        this.changeSize();
        this.attachMouseWheelEvent();
    }

    ngOnInit() {
        let that = this;
        window.addEventListener("resize", function () { that.changeSize(); });
        this.windowWidth = 2000;
        this.windowLevel = 0;
    }

    /**
     * 置顶指定类型的canvas
     * @param type canvas的类型，overlay、action、cross
     */
    riseZIndexOfCanvas(type) {
        this.imageCanvas.style.zIndex = 0;
        this.overlayCanvas.style.zIndex = 2;
        this.actionCanvas.style.zIndex = 4;
        this.crossCanvas.style.zIndex = 8;
        $('#' + this.id + '-' + type).get(0).style.zIndex = 16;
    }

    /**
     * 改变窗口大小
     */
    private changeSize() {
        let containerWidth = this.container.clientWidth;
        let containerHeight = this.container.clientHeight;
        this.imageCanvas.setAttribute('width', containerWidth);
        this.imageCanvas.setAttribute('height', containerHeight);
        this.crossCanvas.setAttribute('width', containerWidth);
        this.crossCanvas.setAttribute('height', containerHeight);
        this.overlayCanvas.setAttribute('width', containerWidth);
        this.overlayCanvas.setAttribute('height', containerHeight);
        this.actionCanvas.setAttribute('width', containerWidth);
        this.actionCanvas.setAttribute('height', containerHeight);
    }

    /**
     * 给cell容器添加鼠标滚轮事件
     */
    attachMouseWheelEvent() {
        const that = this;
        const scrollFunc = function (e) {
            e = e || window.event;
            let delt = e.wheelDelta / 120;
            if (that.actionInfo.key() === actions.nudge) {
                EventAggregator.Instance().scrollInfo.publish(delt);
            } else {
                that.page(delt);
            }
        };
        this.container.onmousewheel = scrollFunc;
    }

    /**
     * 翻页
     * @param delt 目标页数距离当前页数的间隔
     */
    private page(delt) {
        this.onScroll.emit(delt);
    }

    inputWl(wl) {
        this.windowLevel = Number(wl);
        this.onChangeWwwl.emit([this.windowWidth, this.windowLevel]);
    }

    inputWw(ww) {
        this.windowWidth = Number(ww);
        this.onChangeWwwl.emit([this.windowWidth, this.windowLevel]);
    }

    update(imageData, crossPoint, graphics = null, wwwl = null) {
        if (imageData !== undefined) {
            this.imageData = imageData;
        }
        if (crossPoint !== undefined) {
            this.crossPosition = new Point(crossPoint[0], crossPoint[1]);
        }
        this.graphics = graphics;
        this.updateWWWL(wwwl);
    }

    private updateWWWL(wwwl) {
        if (wwwl === undefined || !Array.isArray(wwwl)) {
            return;
        }
        this.windowWidth = Math.round(wwwl[0]);
        this.windowLevel = Math.round(wwwl[1]);
    }
}
