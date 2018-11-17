import { Component, EventEmitter, Input, OnChanges, Output, SimpleChanges } from '@angular/core';
import { EventAggregator } from '../../../shared/common/event_aggregator';
import { KeyValuePair } from '../../../shared/common/keyvaluepair';
import { LazyExcuteHandler } from '../lazy_excute_handler';
import { CrosslineContainer } from '../shared/container/crossline_container';
import { ConMessageService } from '../shared/service/conMessage.service';
import { Point } from '../shared/tools/point';
declare var $: any;
declare var createjs: any;
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

    lazyExcuteHandler: LazyExcuteHandler;
    name: string;
    windowLevel: any = 0;
    windowWidth: any = 2000;
    actionInfo: any;

    constructor(private conMessage: ConMessageService) {
        this.lazyExcuteHandler = new LazyExcuteHandler();
        this.crossPosition = new Point(0, 0);
        this.actionInfo = new KeyValuePair(actions.locate);
        EventAggregator.Instance().actionInfo.subscribe(value => {
            console.log('cell.component.ts get action info:', value.key())
            this.actionInfo = value;
        });

        EventAggregator.Instance().pageDelta.subscribe(value => {
            this.P2Cross(value);
        });

        EventAggregator.Instance().crossPoint.subscribe(value => {
            if (value[0] !== this.tag) {
                return
            }
            const p = new Point(value[1].x, value[1].y);
            if (this.crossPosition.equals(p)) {
                return;
            }
            this.onLocate.emit([value[1].x, value[1].y]);
            this.crossPosition = p;
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
        this.initCrossLine();
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
                that.P2Cross(delt);
            }
        };
        this.canbas.get(0).onmousewheel = scrollFunc;
    }

    P2Cross(delt) {
        this.onScroll.emit(delt);
    }

    /**
     * 清除所有图元
     */
    clearPri() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.clear));
    }

    /**
     * 画十字线和交点，绑定监听事件
     * @param nix
     * @param niy
     */
    initCrossLine() {
        this.stage = new createjs.Stage(this.crossCanvas);
        var width = this.stage.canvas.width;
        var height = this.stage.canvas.height;
        createjs.Touch.enable(this.stage);
        this.stage.enableMouseOver(50);
        this.stage.mouseMoveOutside = true;

        this.crossLine = new CrosslineContainer(this.stage, this.tag);
        this.updateCrossLine(width / 2, height / 2);
    }

    /**
     * 调整初始的十字线、交点位置，更新stage
     * @param x
     * @param y
     */
    updateCrossLine(x: number, y: number) {
        this.clearmouse();
        this.crossLine.setCenter(x, y);
        this.crossLine.update();
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

    clearmouse() {
        // $('#threebmp').removeClass();
        this.canbas.get(0).onmousedown = null;
    }

    addPanEvent() {
        let that = this;
        // $('#threebmp').removeClass().addClass("MoveCursor");
        that.actionCanvas.onmousedown = function (e) {
            let prePos = [e.clientX, e.clientY];
            console.log('enter pan mouse down');
            that.actionCanvas.onmousemove = function (e) {
                if (!that.lazyExcuteHandler.canExcuteByCount()) return;
                let curPos = [e.clientX, e.clientY];
                that.onPan.emit([that.tag, prePos, curPos]);
                prePos = curPos;
            };
            that.actionCanvas.onmouseup = function () {
                that.actionCanvas.onmousemove = null;
                that.actionCanvas.onmouseup = null;
            };
        }
    }

    addZoomEvent() {
        let that = this;
        // $('#threebmp').removeClass().addClass("ZoomCursor");
        that.actionCanvas.onmousedown = function (e) {
            let zoom_factor = 0;
            let preY = e.clientY;
            that.actionCanvas.onmousemove = function (e) {
                if (!that.lazyExcuteHandler.canExcuteByCount()) return;
                let curY = e.clientY;
                let shiftY = curY - preY;
                preY = curY;
                if (shiftY >= 0) {
                    zoom_factor = 1.0 + shiftY * 1.0 / 120
                } else {
                    zoom_factor = 1.0 / (1.0 - shiftY * 1.0 / 120)
                }

                that.onZoom.emit([that.tag, zoom_factor]);
            }
            that.actionCanvas.onmouseup = function (e) {
                that.actionCanvas.onmousemove = null;
                that.actionCanvas.onmouseup = null;
            }
        }
    }

    addRotateEvent() {
        const that = this;
        that.actionCanvas.onmousedown = function (e) {
            let prePos = [e.clientX, e.clientY];
            that.actionCanvas.onmousemove = function (e) {
                if (!that.lazyExcuteHandler.canExcuteByCount()) return;
                let curPos = [e.clientX, e.clientY];
                that.onRotate.emit([that.tag, prePos, curPos]);
                prePos = curPos;
            };
            that.actionCanvas.onmouseup = function () {
                that.actionCanvas.onmousemove = null;
                that.actionCanvas.onmouseup = null;
            };
        };
    }

    addChangeWlEvent() {
        const that = this;
        that.actionCanvas.onmousedown = function (e) {
            let ww_factor = 0;
            let wl_factor = 0;
            let preX = e.clientX;
            let preY = e.clientY;
            that.actionCanvas.onmousemove = function (e) {
                if (!that.lazyExcuteHandler.canExcuteByCount()) return;
                let curX = e.clientX;
                let curY = e.clientY;
                let shiftY = curY - preY;
                if (shiftY >= 0) {
                    ww_factor = 1.0 + shiftY * 1.0 / 1000
                } else {
                    ww_factor = 1.0 / (1.0 - shiftY * 1.0 / 1000)
                }
                wl_factor = (preX - curX) * 1.0 / 2000;
                preX = curX;
                preY = curY;
                const level = Math.round(that.windowLevel + that.windowWidth * wl_factor);
                const width = Math.round(that.windowWidth * ww_factor);
                if (level == that.windowLevel && width == that.windowWidth) {
                    return;
                }
                that.onChangeWwwl.emit([width, level, 'true']);
            };
            that.actionCanvas.onmouseup = function (e) {
                that.actionCanvas.onmousemove = null;
                that.actionCanvas.onmouseup = null;
            };
        };
    }

    cellUpdate(imageData, crossPoint, graphics = null, wwwl = null) {
        if (imageData != null) {
            this.updateImage(imageData);
        }
        if (crossPoint != null) {
            this.updateCrossLine(crossPoint[0], crossPoint[1]);
        }
        this.updateGraphics(graphics);
        this.updateWWWL(wwwl);
    }

    updateGraphics(graphics) {
        this.conMessage.setGraphics([this.tag, graphics]);
    }

    updateImage(imageData) {
        const img1 = new Image();
        const base64Header = 'data:image/jpeg;base64,';
        const imgData1 = base64Header + imageData;
        img1.src = imgData1;
        const that = this;
        img1.onload = function () {
            const ctx = that.imageCanvas.getContext('2d');
            ctx.clearRect(0, 0, that.imageCanvas.width, that.imageCanvas.height);
            ctx.drawImage(img1, 0, 0, that.imageCanvas.width, that.imageCanvas.height);
        };
    }

    updateWWWL(wwwl) {
        if (wwwl === undefined || !Array.isArray(wwwl)) {
            return;
        }
        this.windowWidth = Math.round(wwwl[0]);
        this.windowLevel = Math.round(wwwl[1]);
    }
}
