import { Component, EventEmitter, Input, OnChanges, Output, SimpleChanges } from '@angular/core';
import { EventAggregator } from '../../../shared/common/event_aggregator';
import { Point } from '../shared/tools/point';
import { CellModel } from '../shared/model/cell.model';
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

    // Data model of cell
    @Input() model: CellModel = new CellModel();

    @Output() onLocate: EventEmitter<any> = new EventEmitter<any>();
    @Output() onScroll: EventEmitter<any> = new EventEmitter<any>();
    @Output() onZoom: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onPan: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onRotate: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onChangeWwwl: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();

    constructor() {
        this.id = 'cell';

        this.model = new CellModel();

        EventAggregator.Instance().pageDelta.subscribe(value => {
            this.page(value);
        });

        EventAggregator.Instance().crossPoint.subscribe(value => {
            if (value[0] !== this.model.tag) {
                return;
            }
            const p = new Point(value[1].x, value[1].y);
            if (this.model.crossM.point.equals(p)) {
                return;
            }
            this.onLocate.emit([value[1].x, value[1].y]);
            this.model.crossM.point = p;
        });

        EventAggregator.Instance().eventData.subscribe(value => {
            if (value === undefined || value.length != 2) {
                return;
            }
            const action = value[0];
            const data = value[1];
            if (data[0] !== this.model.tag) {
                return;
            }
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
                    const level = Math.round(this.model.imageM.windowLevel + this.model.imageM.windowWidth * data[2]);
                    const width = Math.round(this.model.imageM.windowWidth * data[1]);
                    if (level == this.model.imageM.windowLevel && width == this.model.imageM.windowWidth) {
                        return;
                    }
                    console.log(data, 'old wwwl', this.model.imageM.windowWidth, this.model.imageM.windowLevel, 'new wwwl', width, level)
                    this.model.imageM.setWwwl([width, level]);
                    this.onChangeWwwl.emit([width, level]);
                    break;
                default:
                    break;
            }
        });
    }

    //#region life-cycle hook methods
    ngOnInit() {
        let that = this;
        window.addEventListener("resize", function () { that.changeSize(); });
        this.model.imageM.windowWidth = 2000;
        this.model.imageM.windowLevel = 0;
    }

    ngAfterViewInit() {
        // Set id for all canvas and their container
        this.container = $('#' + this.id + '-container').get(0);
        this.imageCanvas = $('#' + this.id + '-image').get(0);
        this.crossCanvas = $('#' + this.id + '-cross').get(0);
        this.overlayCanvas = $('#' + this.id + '-overlay').get(0);
        this.actionCanvas = $('#' + this.id + '-action').get(0);

        this.changeSize();

        this.attachMouseWheelEvent();
    }
    //#endregion

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
            let delta = e.wheelDelta / 120;
            if (that.model.actionInfo.key() === actions.nudge) {
                EventAggregator.Instance().faderRadiusDelta.publish(delta);
            } else {
                that.page(delta);
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
        this.model.imageM.windowLevel = Number(wl);
        this.onChangeWwwl.emit([this.model.imageM.windowWidth, this.model.imageM.windowLevel]);
    }

    inputWw(ww) {
        this.model.imageM.windowWidth = Number(ww);
        this.onChangeWwwl.emit([this.model.imageM.windowWidth, this.model.imageM.windowLevel]);
    }
}
