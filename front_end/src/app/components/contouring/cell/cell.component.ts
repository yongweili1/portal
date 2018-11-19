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
export class CellComponent implements OnChanges {
    imageCanvas: any; canbas: any; crossCanvas: any; actionCanvas: any; overlayCanvas: any;

    @Input() tag: any;
    @Input() hasLoadVolume;

    containerWidth = $('.containe').width();
    containerHeight = $('.containe').height();
    viewportWidth = Math.floor(this.containerWidth / 2) * 2;
    viewportHeight = Math.floor(this.containerHeight / 2) * 2;

    // 十字线
    stage: any; crossLine: CrosslineContainer; crossPosition: Point;

    @Output() onLocate: EventEmitter<any> = new EventEmitter<any>();
    @Output() onScroll: EventEmitter<any> = new EventEmitter<any>();
    @Output() onZoom: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onPan: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onRotate: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onChangeWwwl: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();

    name: string;
    windowLevel: any = 0;
    windowWidth: any = 2000;
    actionInfo: any;

    constructor(private conMessage: ConMessageService) {
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
                    this.onChangeWwwl.emit([width, level, 'true']);
                    break;
                default:
                    break;
            }
        });
    }

    ngOnChanges(changes: SimpleChanges) {
        this.name = this.tag;

        if (this.tag == "transverse") {
            this.imageCanvas = $(".a_class .imageCanvas").get(0);
            this.canbas = $(".a_class");
            this.crossCanvas = $(".a_class .crossCanvas").get(0);
            this.overlayCanvas = $(".a_class .overlayCanvas").get(0);
            this.actionCanvas = $(".a_class #actionCanvas").get(0);
            this.canbas.find(".mpr").text('Transverse');
        }
        if (this.tag == "coronal") {
            this.imageCanvas = $(".b_class .imageCanvas").get(0);
            this.canbas = $(".b_class");
            this.crossCanvas = $(".b_class .crossCanvas").get(0);
            this.overlayCanvas = $(".b_class .overlayCanvas").get(0);
            this.actionCanvas = $(".b_class #actionCanvas").get(0);
            this.canbas.find(".mpr").text('Coronal');
        }
        if (this.tag == "saggital") {
            this.imageCanvas = $(".c_class .imageCanvas").get(0);
            this.canbas = $(".c_class");
            this.crossCanvas = $(".c_class .crossCanvas").get(0);
            this.overlayCanvas = $(".c_class .overlayCanvas").get(0);
            this.actionCanvas = $(".c_class #actionCanvas").get(0);
            this.canbas.find(".mpr").text('Sagittal');
        }
        this.calcviewportsize();
        this.windowAddMouseWheel();
    }

    ngOnInit() {
        let that = this;
        $(window).resize(function () {
            that.calcviewportsize();
            console.log("=== resize ===")
        });
        this.windowWidth = 2000;
        this.windowLevel = 0;
    }

    // 设置和区分canvas窗口大小
    calcviewportsize() {
        if (this.tag === 'transverse') {
            this.containerWidth = $('.a_class .containe').width();
            this.containerHeight = $('.a_class .containe').height();
        }
        if (this.tag === 'coronal') {
            this.containerWidth = $('.b_class .containe').width();
            this.containerHeight = $('.b_class .containe').height();
        }
        if (this.tag === 'saggital') {
            this.containerWidth = $('.c_class .containe').width();
            this.containerHeight = $('.c_class .containe').height();
        }
        this.viewportWidth = Math.floor(this.containerWidth / 2) * 2;
        this.viewportHeight = Math.floor(this.containerHeight / 2) * 2;
        this.imageCanvas.setAttribute('width', this.viewportWidth);
        this.imageCanvas.setAttribute('height', this.viewportHeight);
        this.crossCanvas.setAttribute('width', this.viewportWidth); // 十字线的canvas
        this.crossCanvas.setAttribute('height', this.viewportHeight);
        this.overlayCanvas.setAttribute('width', this.viewportWidth); // 图元操作绘画层的canvas
        this.overlayCanvas.setAttribute('height', this.viewportHeight);
        this.actionCanvas.setAttribute('width', this.viewportWidth); // nuge的canvas
        this.actionCanvas.setAttribute('height', this.viewportHeight);

        this.crossLine = new CrosslineContainer(this.crossCanvas, this.tag);
    }

    /**
     * 设置canvas的Z-index
     * @param canvasid
     * @param targetindex
     */
    SetCanvasIndex(canvasid: any, targetindex: number) {
        if (this.tag == "transverse") {
            $(`.a_class ${canvasid}`).get(0).style.zIndex = targetindex;
        }
        if (this.tag == "coronal") {
            $(`.b_class ${canvasid}`).get(0).style.zIndex = targetindex;
        }
        if (this.tag == "saggital") {
            $(`.c_class ${canvasid}`).get(0).style.zIndex = targetindex;
        }
    }

    windowAddMouseWheel() {
        const that = this;
        let delt: any;
        const scrollFunc = function (e) {
            e = e || window.event;
            delt = e.wheelDelta / 120;
            if (that.actionInfo.key() === actions.nudge) {
                EventAggregator.Instance().scrollInfo.publish(delt);
            } else {
                that.page(delt);
            }
        };
        this.canbas.get(0).onmousewheel = scrollFunc;
    }

    page(delt) {
        this.onScroll.emit(delt);
    }

    /**
     * 清除所有图元
     */
    clearContours() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.clear));
    }

    onChangewlww(inval) {
        console.log(inval)
        let wwwlStrArray = inval.split(',')
        let wwwlIntArray = []
        let flag = 'true'
        wwwlStrArray.forEach(element => {
            try {
                wwwlIntArray.push(Number(element));
            }
            catch (err) {
                console.log('cant convert to number');
                flag = 'false'
            }
        });
        if (typeof (wwwlIntArray[0]) == 'number' && wwwlIntArray[0] > 0) {
            this.windowWidth = wwwlIntArray[0]
        } else {
            flag = 'false'
        }
        if (typeof (wwwlIntArray[1]) == 'number') {
            this.windowLevel = wwwlIntArray[1]
        } else {
            flag = 'false'
        }
        this.onChangeWwwl.emit([this.windowWidth, this.windowLevel, flag]);
    }

    update(imageData, crossPoint, graphics = null, wwwl = null) {
        if (imageData !== undefined) {
            this.updateImage(imageData);
        }
        if (crossPoint !== undefined) {
            this.updateCrossLine(crossPoint[0], crossPoint[1]);
        }
        this.updateGraphics(graphics);
        this.updateWWWL(wwwl);
    }

    updateImage(imageData) {
        const img = new Image();
        const base64Header = 'data:image/jpeg;base64,';
        const imgData1 = base64Header + imageData;
        img.src = imgData1;
        const that = this;
        img.onload = function () {
            const ctx = that.imageCanvas.getContext('2d');
            ctx.clearRect(0, 0, that.imageCanvas.width, that.imageCanvas.height);
            ctx.drawImage(img, 0, 0, that.imageCanvas.width, that.imageCanvas.height);
        };
    }

    updateCrossLine(x: number, y: number) {
        this.crossLine.setCenter(x, y);
        this.crossLine.update();
    }

    updateGraphics(graphics) {
        this.conMessage.setGraphics([this.tag, graphics]);
    }

    updateWWWL(wwwl) {
        if (wwwl === undefined || !Array.isArray(wwwl)) {
            return;
        }
        this.windowWidth = Math.round(wwwl[0]);
        this.windowLevel = Math.round(wwwl[1]);
    }
}
