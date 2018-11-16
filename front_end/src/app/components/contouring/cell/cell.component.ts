import { Component, EventEmitter, Input, OnChanges, Output, SimpleChanges } from '@angular/core';
import { EventAggregator } from '../../../shared/common/event_aggregator';
import { KeyValuePair } from '../../../shared/common/keyvaluepair';
import { LazyExcuteHandler } from '../lazy_excute_handler';
import { Hitbar } from '../shared/overlay/hitbar';
import { ConMessageService } from '../shared/service/conMessage.service';
declare var $: any;
declare var createjs: any;
declare var actions: any;

@Component({
    selector: 'mpt-cell',
    templateUrl: './cell.component.html',
    styleUrls: ['./cell.component.less']
})
export class CellComponent implements OnChanges {
    imageCanvas: any; canbas: any; crossCanvas: any; toolsCavas: any; overlayCanvas: any;

    @Input() tag: any; @Input() wl: any = 0; @Input() ww: any = 2000;
    @Input() hasLoadVolume;

    containerWidth = $('.containe').width();
    containerHeight = $('.containe').height();
    viewportWidth = Math.floor(this.containerWidth / 2) * 2;
    viewportHeight = Math.floor(this.containerHeight / 2) * 2;
    // 十字线
    stage: any; horizontalLine: any; verticalLine: any; crossPoint: any; postPoint: any;
    nix = 0.5; niy = 0.5;

    @Output() onLocate: EventEmitter<any> = new EventEmitter<any>();
    @Output() onScroll: EventEmitter<any> = new EventEmitter<any>();
    @Output() onZoom: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onPan: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onRotate: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() onChangeWwwl: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();

    @Output() wwwlReq2: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();

    lazyExcuteHandler: LazyExcuteHandler;
    name: string;
    viewWL: any;
    viewWW: any;
    actionInfo: any;

    constructor(
        private conMessage: ConMessageService,
    ) {
        this.lazyExcuteHandler = new LazyExcuteHandler();

        this.actionInfo = new KeyValuePair(actions.locate);
        EventAggregator.Instance().actionInfo.subscribe(value => {
            console.log('cell.component.ts get action info:', value.key())
            this.actionInfo = value;
        });

        EventAggregator.Instance().pageDelta.subscribe(value => {
            this.P2Cross(value);
        });
    }

    ngOnChanges(changes: SimpleChanges) {
        this.name = this.tag;

        if (this.tag == "transverse") {
            this.imageCanvas = $(".a_class .imageCanvas").get(0);
            this.canbas = $(".a_class");
            this.crossCanvas = $(".a_class .crossCanvas").get(0);
            this.overlayCanvas = $(".a_class .overlayCanvas").get(0);
            this.toolsCavas = $(".a_class #toolsCanvas").get(0);
            this.canbas.find(".mpr").text('Transverse');
        }
        if (this.tag == "coronal") {
            this.imageCanvas = $(".b_class .imageCanvas").get(0);
            this.canbas = $(".b_class");
            this.crossCanvas = $(".b_class .crossCanvas").get(0);
            this.overlayCanvas = $(".b_class .overlayCanvas").get(0);
            this.toolsCavas = $(".b_class #toolsCanvas").get(0);
            this.canbas.find(".mpr").text('Coronal');
        }
        if (this.tag == "saggital") {
            this.imageCanvas = $(".c_class .imageCanvas").get(0);
            this.canbas = $(".c_class");
            this.crossCanvas = $(".c_class .crossCanvas").get(0);
            this.overlayCanvas = $(".c_class .overlayCanvas").get(0);
            this.toolsCavas = $(".c_class #toolsCanvas").get(0);
            this.canbas.find(".mpr").text('Sagittal');
        }
        this.calcviewportsize();
        this.windowAddMouseWheel(this.tag);
    }

    ngOnInit() {
        let that = this;
        $(window).resize(function () {
            that.calcviewportsize();
            console.log("=== resize ===")
        });
        this.viewWW = 2000;
        this.viewWL = 0;
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
        this.toolsCavas.setAttribute('width', this.viewportWidth); // nuge的canvas
        this.toolsCavas.setAttribute('height', this.viewportHeight);
        this.drawCross(this.nix, this.niy, this.canbas.get(0));
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

    // 翻页
    windowAddMouseWheel(tag) {
        const that = this;
        let delt: any;
        const scrollFunc = function (e) {
            e = e || window.event;
            delt = e.wheelDelta / 120;
            if (that.actionInfo.key() === actions.nudge) {
                console.log('scroll func fader radius', delt)
                EventAggregator.Instance().scrollInfo.publish(delt);
            } else {
                console.log('scroll func page', delt);
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
     * @param loca 
     */
    drawCross(nix, niy, loca) {
        this.stage = new createjs.Stage(this.crossCanvas);
        var width = this.stage.canvas.width;
        var height = this.stage.canvas.height;
        createjs.Touch.enable(this.stage);
        this.stage.enableMouseOver(50);
        this.stage.mouseMoveOutside = true;

        this.horizontalLine = new createjs.Shape(); // 横线
        if (this.tag == "transverse") {
            this.horizontalLine.graphics.beginStroke("#2196F3").setStrokeStyle(1, "round").moveTo(0, 0).lineTo(width, 0);
        }
        if (this.tag == "coronal") {
            this.horizontalLine.graphics.beginStroke("#F44336").setStrokeStyle(1, "round").moveTo(0, 0).lineTo(width, 0);
        }
        if (this.tag == "saggital") {
            this.horizontalLine.graphics.beginStroke("#F44336").setStrokeStyle(1, "round").moveTo(0, 0).lineTo(width, 0);
        }
        let horizontalHitbar = new Hitbar()
        horizontalHitbar.graphics.moveTo(0, 0).lineTo(width, 0);
        this.horizontalLine.hitArea = horizontalHitbar
        this.horizontalLine.cursor = "url('/assets/img/vertical.cur'),auto";

        this.verticalLine = new createjs.Shape(); // 竖线
        if (this.tag == "transverse") {
            this.verticalLine.graphics.beginStroke("#CDDC39").setStrokeStyle(1, "round").moveTo(0, 0).lineTo(0, height);
        }
        if (this.tag == "coronal") {
            this.verticalLine.graphics.beginStroke("#CDDC39").setStrokeStyle(1, "round").moveTo(0, 0).lineTo(0, height);
        }
        if (this.tag == "saggital") {
            this.verticalLine.graphics.beginStroke("#2196F3").setStrokeStyle(1, "round").moveTo(0, 0).lineTo(0, height);
        }
        var verticalHitbar = new Hitbar();
        verticalHitbar.graphics.moveTo(0, 0).lineTo(0, height);
        this.verticalLine.hitArea = verticalHitbar;
        this.verticalLine.cursor = "url('/assets/img/horizontal.cur'),auto";

        this.crossPoint = new createjs.Shape();// 交点
        this.crossPoint.graphics.beginFill("white").drawCircle(0, 0, 8);
        this.crossPoint.alpha = 0.2;
        this.crossPoint.cursor = "url('/assets/img/move.cur'),auto";
        this.stage.addChild(this.verticalLine);
        this.stage.addChild(this.horizontalLine);
        this.stage.addChild(this.crossPoint);
        var width = this.stage.canvas.width;
        var height = this.stage.canvas.height;
        this.locate(nix * width, niy * height);

        this.stage.canvas.addEventListener('mousedown', this.handleMouseDown.bind(this))
        this.horizontalLine.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.verticalLine.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.crossPoint.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.horizontalLine.addEventListener("pressup", this.handlePressUp.bind(this));
        this.verticalLine.addEventListener("pressup", this.handlePressUp.bind(this));
        this.crossPoint.addEventListener("pressup", this.handlePressUp.bind(this));
    }

    handleMouseDown(evt) {
        if (evt.currentTarget == this.verticalLine
            || evt.currentTarget == this.horizontalLine
            || evt.currentTarget == this.crossPoint) return;
        this.crossPoint.x = evt.offsetX;
        this.crossPoint.y = evt.offsetY;
        this.locate(this.crossPoint.x, this.crossPoint.y);
        this.getposition(this.crossPoint.x, this.crossPoint.y);
        this.stage.update();
    }

    /**
     * 调整初始的十字线、交点位置，更新stage
     * @param width
     * @param height
     */
    locate(x: number, y: number) {
        this.clearmouse();
        this.verticalLine.x = x;
        this.horizontalLine.y = y;
        this.crossPoint.x = this.verticalLine.x;
        this.crossPoint.y = this.horizontalLine.y;
        this.stage.update();
    }

    handlePressMove(evt) {
        if (!this.lazyExcuteHandler.canExcuteByCount()) return;
        if (evt.currentTarget == this.verticalLine) {//竖线
            evt.currentTarget.x = this.crossPoint.x = evt.stageX;
        } else if (evt.currentTarget == this.horizontalLine) {//横线
            evt.currentTarget.y = this.crossPoint.y = evt.stageY;
        } else if (evt.currentTarget == this.crossPoint) {
            evt.currentTarget.x = this.verticalLine.x = evt.stageX;
            evt.currentTarget.y = this.horizontalLine.y = evt.stageY;
        }
        this.getposition(this.crossPoint.x, this.crossPoint.y);
        this.stage.update();
    }

    handlePressUp(evt) {
        if (evt.currentTarget == this.verticalLine) {//竖线
            evt.currentTarget.x = this.crossPoint.x = evt.stageX;
        }
        if (evt.currentTarget == this.horizontalLine) {//横线
            evt.currentTarget.y = this.crossPoint.y = evt.stageY;
        }
        if (evt.currentTarget == this.crossPoint) {
            evt.currentTarget.x = this.crossPoint.x = evt.stageX;
            evt.currentTarget.y = this.crossPoint.y = evt.stageY;
        }
        this.getposition(this.crossPoint.x, this.crossPoint.y);
        this.stage.update();
        this.locate(this.crossPoint.x, this.crossPoint.y)
    }

    getposition(x, y) {
        var point = new Array(x, y);
        console.log(point);
        this.onLocate.emit(point);
    }

    onChangewlww(inval) {
        console.log(inval)
        let wwwlStrArray = inval.split(',')
        let wwwlIntArray = []
        let flag = 'true'
        wwwlStrArray.forEach(element => {
            ''
            try {
                wwwlIntArray.push(Number(element));
            }
            catch (err) {
                console.log('cant convert to number');
                flag = 'false'
            }
        });
        if (typeof (wwwlIntArray[0]) == 'number' && wwwlIntArray[0] > 0) {
            this.ww = wwwlIntArray[0]
        }
        else {
            flag = 'false'
        }
        if (typeof (wwwlIntArray[1]) == 'number') {
            this.wl = wwwlIntArray[1]
        }
        else {
            flag = 'false'
        }
        this.wwwlReq2.emit([this.ww, this.wl, flag]);
    }

    clearmouse() {
        // $('#threebmp').removeClass();
        this.canbas.get(0).onmousedown = null;
    }

    addPanEvent() {
        let that = this;
        // $('#threebmp').removeClass().addClass("MoveCursor");
        that.toolsCavas.onmousedown = function (e) {
            let prePos = [e.clientX, e.clientY];
            console.log('enter pan mouse down');
            that.toolsCavas.onmousemove = function (e) {
                if (!that.lazyExcuteHandler.canExcuteByCount()) return;
                let curPos = [e.clientX, e.clientY];
                that.onPan.emit([that.tag, prePos, curPos]);
                prePos = curPos;
            };
            that.toolsCavas.onmouseup = function () {
                that.toolsCavas.onmousemove = null;
                that.toolsCavas.onmouseup = null;
            };
        }
    }

    addZoomEvent() {
        let that = this;
        // $('#threebmp').removeClass().addClass("ZoomCursor");
        that.toolsCavas.onmousedown = function (e) {
            let zoom_factor = 0;
            let preY = e.clientY;
            that.toolsCavas.onmousemove = function (e) {
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
            that.toolsCavas.onmouseup = function (e) {
                that.toolsCavas.onmousemove = null;
                that.toolsCavas.onmouseup = null;
            }
        }
    }

    addRotateEvent() {
        const that = this;
        that.toolsCavas.onmousedown = function (e) {
            let prePos = [e.clientX, e.clientY];
            that.toolsCavas.onmousemove = function (e) {
                if (!that.lazyExcuteHandler.canExcuteByCount()) return;
                let curPos = [e.clientX, e.clientY];
                that.onRotate.emit([that.tag, prePos, curPos]);
                prePos = curPos;
            };
            that.toolsCavas.onmouseup = function () {
                that.toolsCavas.onmousemove = null;
                that.toolsCavas.onmouseup = null;
            };
        };
    }

    addChangeWlEvent() {
        const that = this;
        that.toolsCavas.onmousedown = function (e) {
            let ww_factor = 0;
            let wl_factor = 0;
            let preX = e.clientX;
            let preY = e.clientY;
            that.toolsCavas.onmousemove = function (e) {
                if (!that.lazyExcuteHandler.canExcuteByCount()) return;
                let curX = e.clientX;
                let curY = e.clientY;
                let shiftY = curY - preY;
                if (shiftY >= 0) {
                    ww_factor = 1.0 + shiftY * 1.0 / 120
                } else {
                    ww_factor = 1.0 / (1.0 - shiftY * 1.0 / 120)
                }
                wl_factor = (preX - curX) * 1.0 / 240;
                preX = curX;
                preY = curY;
                that.onChangeWwwl.emit([that.tag, ww_factor, wl_factor]);
            };
            that.toolsCavas.onmouseup = function (e) {
                that.toolsCavas.onmousemove = null;
                that.toolsCavas.onmouseup = null;
            };
        };
    }

    cellUpdate(imageData, crossPoint, graphics = null, wwwl = null) {
        if (imageData != null) {
            this.updateImage(imageData);
        }
        if (crossPoint != null) {
            this.locate(crossPoint[0], crossPoint[1]);
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
        if (wwwl != null && wwwl !== undefined && Array.isArray(wwwl) === true) {
            this.viewWW = Math.round(wwwl[0]);
            this.viewWL = Math.round(wwwl[1]);
        }
    }
}
