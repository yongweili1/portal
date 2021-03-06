import { AfterViewInit, Component, EventEmitter, Input, OnDestroy, OnInit, Output } from '@angular/core';
import { Subscription } from 'rxjs';
import { EventAggregator } from '../../../shared/common/event_aggregator';
import { CellModel } from '../shared/model/cell.model';
import { Point } from '../shared/tools/point';
import { RoiModel } from '../shared/model/roi.model';
import { ActionTypeEnum, CanvasTypeEnum, ShapeTypeEnum } from '../../../shared/models/enums';

declare var $: any;

@Component({
    selector: 'mpt-cell',
    templateUrl: './cell.component.html',
    styleUrls: ['./cell.component.less']
})
export class CellComponent implements OnInit, AfterViewInit, OnDestroy {
    id: string;

    pageDeltaSubscriber: Subscription;
    crossPointSubscriber: Subscription;
    eventDataSubscriber: Subscription;

    container: any; imageCanvas: any; crossCanvas: any; actionCanvas: any; overlayCanvas: any;

    // Data model of cell
    @Input() model: CellModel = new CellModel();
    @Input() roi: any = new RoiModel();
    @Input() rois: any = new Array<RoiModel>();
    @Input() actionType: ActionTypeEnum = ActionTypeEnum.unknown;
    @Input() shapeType: ShapeTypeEnum = ShapeTypeEnum.unknown;

    @Output() onLocate: EventEmitter<any> = new EventEmitter<any>();
    @Output() onScroll: EventEmitter<any> = new EventEmitter<any>();
    @Output() onZoom: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onPan: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onRotate: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onChangeWwwl: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();

    constructor() {
        this.id = 'cell';

        this.model = new CellModel();

        this.pageDeltaSubscriber = EventAggregator.Instance().pageDelta.subscribe(value => {
            if (value !== undefined && value[0] === this.model.tag) {
                this.page(value[1]);
            }
        });

        this.crossPointSubscriber = EventAggregator.Instance().crossPoint.subscribe(value => {
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

        this.eventDataSubscriber = EventAggregator.Instance().eventData.subscribe(value => {
            if (value === undefined || value.length !== 2) {
                return;
            }
            const action = value[0];
            const data = value[1];
            if (data[0] !== this.model.tag) {
                return;
            }
            switch (action) {
                case ActionTypeEnum.locate:
                    break;
                case ActionTypeEnum.pan:
                    this.onPan.emit(data);
                    break;
                case ActionTypeEnum.zoom:
                    this.onZoom.emit(data);
                    break;
                case ActionTypeEnum.rotate:
                    this.onRotate.emit(data);
                    break;
                case ActionTypeEnum.window:
                    const level = Math.round(this.model.imageM.windowLevel + this.model.imageM.windowWidth * data[2]);
                    const width = Math.round(this.model.imageM.windowWidth * data[1]);
                    if (level === this.model.imageM.windowLevel && width === this.model.imageM.windowWidth) {
                        return;
                    }
                    console.log(data, 'old wwwl', this.model.imageM.windowWidth, this.model.imageM.windowLevel, 'new wwwl', width, level);
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
        const that = this;
        window.addEventListener('resize', function () { that.changeSize(); });
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

    ngOnDestroy() {
        // 防止重复订阅
        try {
            this.pageDeltaSubscriber.unsubscribe();
            this.crossPointSubscriber.unsubscribe();
            this.eventDataSubscriber.unsubscribe();
        } catch (error) {
            console.error(error.message);
        }
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
        const canvasId = '#' + this.id + '-' + CanvasTypeEnum[type];
        console.log(canvasId);
        $(canvasId).get(0).style.zIndex = 16;
    }

    /**
     * 改变窗口大小
     */
    private changeSize() {
        const containerWidth = this.container.clientWidth;
        const containerHeight = this.container.clientHeight;
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
            const delta = e.wheelDelta / 120;
            if (that.actionType === ActionTypeEnum.nudge) {
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
