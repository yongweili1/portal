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
    selector: 'mpt-pic-transverse',
    templateUrl: './pic-transverse.component.html',
    styleUrls: ['./pic-transverse.component.less']
})
export class PicTransverseComponent implements OnChanges {

    scale = 1.0; transX = 0.0; transY = 0.0;
    canvas: any; canbas: any; crosscan: any; toolsvas: any; primitivecan: any; primitivedrawcan: any;
    @Input() tag: any; @Input() imageWidth; @Input() imageHeight; @Input() pageindex;
    @Input() wl: any = 0; @Input() ww: any = 2000; // 窗宽
    @Input() spacingX; @Input() spacingY; @Input() gap; @Input() sliceAll;
    @Input() pixelRepresentation; @Input() rescaleSlope; @Input() rescaleIntercept;
    @Input() firstImagePosition; @Input() lastImagePosition; @Input() hasLoadVolume;
    containerWidth = $('.containe').width(); containerHeight = $('.containe').height();
    viewportWidth = Math.floor(this.containerWidth / 2) * 2; viewportHeight = Math.floor(this.containerHeight / 2) * 2;
    unts: any; ctx: any; wlx: any; wwx: any; wlold: any; wwold: any;// 窗位窗宽
    fitImageWidth: any; fitImageHeight: any; sx: any; mx: any; my: any;
    pageindexit: any;
    // 十字线
    stage: any; horizontalLine: any; verticalLine: any; crossPoint: any; postPoint: any;
    nix = 0.5; niy = 0.5;
    mpr2Patient: any; opM3: any;
    roiContourSets = new Array();
    Line: any;
    NIdata: any = { ctpointX: 0.5, ctpointY: 0.5 };
    public affineMat3; public affineMat4;
    @Output() changeCross: EventEmitter<any> = new EventEmitter<any>();
    @Output() twoCross: EventEmitter<any> = new EventEmitter<any>();
    @Output() message: EventEmitter<any> = new EventEmitter<any>();
    @Output() scroll: EventEmitter<any> = new EventEmitter<any>();
    @Output() zoomReq: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() panReq: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() rotateReq: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() wwwlReq: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    @Output() wwwlReq2: EventEmitter<Array<any>> = new EventEmitter<Array<any>>();
    curAction: any;
    focus: any; display: any;
    lazyExcuteHandler: LazyExcuteHandler;
    name: string;
    viewWL: any;
    viewWW: any;
    actionInfo: any;

    constructor(
        private conMessage: ConMessageService,
    ) {
        this.lazyExcuteHandler = new LazyExcuteHandler();
        this.curAction = 'croselect';

        this.actionInfo = new KeyValuePair(actions.locate);
        EventAggregator.Instance().actionInfo.subscribe(value => {
            console.log('pic-transverse.component.ts get action info:', value.key())
            this.actionInfo = value;
        });

        EventAggregator.Instance().pageDelta.subscribe(value => {
            this.P2Cross(value);
        });
    }

    ngOnChanges(changes: SimpleChanges) {
        this.name = this.tag
        this.wlold = this.wl;//窗位
        this.wwold = this.ww;// 窗宽
        this.pageindexit = this.pageindex * 2;
        if (this.tag == "transverse") {
            this.canvas = $(".a_class .icanvas").get(0);
            this.canbas = $(".a_class");
            this.crosscan = $(".a_class .crosscan").get(0);
            this.primitivedrawcan = $(".a_class .primitivedrawcan").get(0);
            this.toolsvas = $(".a_class #toolsCanvas").get(0);
            this.canbas.find(".mpr").text('Transverse');
        }
        if (this.tag == "coronal") {
            this.canvas = $(".b_class .icanvas").get(0);
            this.canbas = $(".b_class");
            this.crosscan = $(".b_class .crosscan").get(0);
            this.primitivedrawcan = $(".b_class .primitivedrawcan").get(0);
            this.toolsvas = $(".b_class #toolsCanvas").get(0);
            this.canbas.find(".mpr").text('Coronal');
            var myCanvas = $('.b_class #canvas-frame').get(0);
            var lightPoint = new Array(0, 0, 100);
        }
        if (this.tag == "saggital") {
            this.canvas = $(".c_class .icanvas").get(0);
            this.canbas = $(".c_class");
            this.crosscan = $(".c_class .crosscan").get(0);
            this.primitivedrawcan = $(".c_class .primitivedrawcan").get(0);
            this.toolsvas = $(".c_class #toolsCanvas").get(0);
            this.canbas.find(".mpr").text('Sagittal');
            var myCanvas = $('.c_class #canvas-frame').get(0);
            var lightPoint = new Array(100, 0, 0)
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
        this.canvas.setAttribute('width', this.viewportWidth);
        this.canvas.setAttribute('height', this.viewportHeight);
        this.crosscan.setAttribute('width', this.viewportWidth); // 十字线的canvas
        this.crosscan.setAttribute('height', this.viewportHeight);
        this.primitivedrawcan.setAttribute('width', this.viewportWidth); // 图元操作绘画层的canvas
        this.primitivedrawcan.setAttribute('height', this.viewportHeight);
        this.toolsvas.setAttribute('width', this.viewportWidth); // nuge的canvas
        this.toolsvas.setAttribute('height', this.viewportHeight);
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
        this.twoCross.emit(delt);
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
        this.stage = new createjs.Stage(this.crosscan);
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
        this.cross(nix * width, niy * height, loca);

        this.stage.canvas.addEventListener('mousedown', this.handleMouseDown.bind(this))
        this.horizontalLine.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.verticalLine.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.crossPoint.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.horizontalLine.addEventListener("pressup", this.handlePressUp.bind(this));
        this.verticalLine.addEventListener("pressup", this.handlePressUp.bind(this));
        this.crossPoint.addEventListener("pressup", this.handlePressUp.bind(this));
    }

    handleMouseDown(evt) {
        if (this.curAction != 'croselect'
            || evt.currentTarget == this.verticalLine
            || evt.currentTarget == this.horizontalLine
            || evt.currentTarget == this.crossPoint) return;
        this.crossPoint.x = evt.offsetX
        this.crossPoint.y = evt.offsetY
        this.cross(this.crossPoint.x, this.crossPoint.y, 1)
        this.getposition(this.crossPoint.x, this.crossPoint.y);
        this.stage.update();
    }

    /**
     * 调整初始的十字线、交点位置，更新stage
     * @param width
     * @param height
     * @param loca
     */
    cross(width, height, loca) {
        this.clearmouse();
        this.horizontalLine.y = height;
        this.verticalLine.x = width;
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
        this.cross(this.crossPoint.x, this.crossPoint.y, 1)
    }

    getposition(x, y) {
        var point = new Array(x, y);
        console.log(point);
        this.changeCross.emit(point);
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
        that.toolsvas.onmousedown = function (e) {
            let prePos = [e.clientX, e.clientY];
            console.log('enter pan mouse down');
            that.toolsvas.onmousemove = function (e) {
                if (!that.lazyExcuteHandler.canExcuteByCount()) return;
                let curPos = [e.clientX, e.clientY];
                that.panReq.emit([that.tag, prePos, curPos]);
                prePos = curPos;
            };
            that.toolsvas.onmouseup = function () {
                that.toolsvas.onmousemove = null;
                that.toolsvas.onmouseup = null;
            };
        }
    }

    addZoomEvent() {
        let that = this;
        // $('#threebmp').removeClass().addClass("ZoomCursor");
        that.toolsvas.onmousedown = function (e) {
            let zoom_factor = 0;
            let preY = e.clientY;
            that.toolsvas.onmousemove = function (e) {
                if (!that.lazyExcuteHandler.canExcuteByCount()) return;
                let curY = e.clientY;
                let shiftY = curY - preY;
                preY = curY;
                if (shiftY >= 0) {
                    zoom_factor = 1.0 + shiftY * 1.0 / 120
                } else {
                    zoom_factor = 1.0 / (1.0 - shiftY * 1.0 / 120)
                }

                that.zoomReq.emit([that.tag, zoom_factor]);
            }
            that.toolsvas.onmouseup = function (e) {
                that.toolsvas.onmousemove = null;
                that.toolsvas.onmouseup = null;
            }
        }
    }

    addRotateEvent() {
        const that = this;
        that.toolsvas.onmousedown = function (e) {
            let prePos = [e.clientX, e.clientY];
            that.toolsvas.onmousemove = function (e) {
                if (!that.lazyExcuteHandler.canExcuteByCount()) return;
                let curPos = [e.clientX, e.clientY];
                that.rotateReq.emit([that.tag, prePos, curPos]);
                prePos = curPos;
            };
            that.toolsvas.onmouseup = function () {
                that.toolsvas.onmousemove = null;
                that.toolsvas.onmouseup = null;
            };
        };
    }

    addChangeWlEvent() {
        const that = this;
        that.toolsvas.onmousedown = function (e) {
            let ww_factor = 0;
            let wl_factor = 0;
            let preX = e.clientX;
            let preY = e.clientY;
            that.toolsvas.onmousemove = function (e) {
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
                that.wwwlReq.emit([that.tag, ww_factor, wl_factor]);
            };
            that.toolsvas.onmouseup = function (e) {
                that.toolsvas.onmousemove = null;
                that.toolsvas.onmouseup = null;
            };
        };
    }

    cellUpdate(imageData, crossPoint, graphics = null, wwwl = null) {
        if (imageData != null) {
            this.drawCanvasPic(imageData);
        }
        if (crossPoint != null) {
            this.cross(crossPoint[0], crossPoint[1], 1);
        }
        this.updateGraphics(graphics);
        this.updateWWWL(wwwl);
    }

    updateGraphics(graphics) {
        this.conMessage.setGraphics([this.tag, graphics]);
    }

    drawCanvasPic(imageData) {
        const img1 = new Image();
        const base64Header = 'data:image/jpeg;base64,';
        const imgData1 = base64Header + imageData;
        img1.src = imgData1;
        const that = this;
        img1.onload = function () {
            const ctx1 = that.canvas.getContext('2d');
            ctx1.clearRect(0, 0, that.canvas.width, that.canvas.height);
            ctx1.drawImage(img1, 0, 0, that.canvas.width, that.canvas.height);
        };
    }

    updateWWWL(wwwl) {
        if (wwwl != null && wwwl !== undefined && Array.isArray(wwwl) === true) {
            this.viewWW = Math.round(wwwl[0]);
            this.viewWL = Math.round(wwwl[1]);
        }
    }

}
