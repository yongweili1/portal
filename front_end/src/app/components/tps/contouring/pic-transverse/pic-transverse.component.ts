import { Component, Input, Output, EventEmitter, SimpleChanges, OnChanges, ViewChild, ElementRef } from '@angular/core';
import { HttpClient } from "@angular/common/http";
import { actionService } from './action.service';
import { StorageService } from '../../shared/service/storage.service';
import { ConMessageService } from '../../shared/service/conMessage.service';
import { SeriesHttpService } from '../../shared/service/seriesHttp.service';
import { glsource } from './glsource.modal';
import { AppConfigService } from '../../../../app.config';
import { LazyExcuteHandler } from '../lazy_excute_handler';
import { Hitbar } from '../../shared/overlay/hitbar';
import { KeyValuePair } from '../../../../shared/common/keyvaluepair';
declare var $: any;
declare var createjs: any;
declare var THREE: any;
declare var MarchingSquaresJS: any;
declare var d3: any;
declare var mat4: any;
declare var vec3: any;
declare var mat3: any;
declare var vec2: any;
declare var vec4: any;
declare var actions: any;

@Component({
    selector: 'mpt-pic-transverse',
    templateUrl: './pic-transverse.component.html',
    styleUrls: ['./pic-transverse.component.less']
})
export class PicTransverseComponent implements OnChanges {

    scale = 1.0; transX = 0.0; transY = 0.0;
    canvas: any; canbas: any; crosscan: any; toolsvas: any; primitivecan: any; primitivedrawcan: any
    @Input() tag: any; @Input() imageWidth; @Input() imageHeight; @Input() pageindex;
    @Input() wl:any = 0; @Input() ww:any = 2000;// 窗宽
    @Input() spacingX; @Input() spacingY; @Input() gap; @Input() sliceAll;
    @Input() pixelRepresentation; @Input() rescaleSlope; @Input() rescaleIntercept;
    @Input() firstImagePosition; @Input() lastImagePosition; @Input() hasLoadVolume;
    containerWidth = $(".containe").width(); containerHeight = $(".containe").height();
    viewportWidth = Math.floor(this.containerWidth / 2) * 2; viewportHeight = Math.floor(this.containerHeight / 2) * 2;
    unts: any; ctx: any; wlx: any; wwx: any; wlold: any; wwold: any;// 窗位窗宽
    fitImageWidth: any; fitImageHeight: any; sx: any; mx: any; my: any;
    pageindexit: any;
    //十字线
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
    glsource = new glsource();
    curAction: any;
    focus: any; display: any;
    lazyExcuteHandler: LazyExcuteHandler;
    name: string;
    viewWL:any;
    viewWW:any;

    constructor(
        public http: HttpClient,
        private conMessage: ConMessageService,
        private storageService: StorageService,
        private actionService: actionService,
        private element: ElementRef,
        private seriesHttpService: SeriesHttpService,
        private appConfig: AppConfigService
    ) {
        this.lazyExcuteHandler = new LazyExcuteHandler();
        this.curAction = 'croselect';
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

    //设置和区分canvas窗口大小
    calcviewportsize() {
        if (this.tag == "transverse") {
            this.containerWidth = $(".a_class .containe").width();
            this.containerHeight = $(".a_class .containe").height();
        }
        if (this.tag == "coronal") {
            this.containerWidth = $(".b_class .containe").width();
            this.containerHeight = $(".b_class .containe").height();
        }
        if (this.tag == "saggital") {
            this.containerWidth = $(".c_class .containe").width();
            this.containerHeight = $(".c_class .containe").height();
        }
        this.viewportWidth = Math.floor(this.containerWidth / 2) * 2;
        this.viewportHeight = Math.floor(this.containerHeight / 2) * 2;
        this.canvas.setAttribute('width', this.viewportWidth);
        this.canvas.setAttribute('height', this.viewportHeight);
        this.crosscan.setAttribute('width', this.viewportWidth);//十字线的canvas
        this.crosscan.setAttribute('height', this.viewportHeight);
        this.primitivedrawcan.setAttribute('width', this.viewportWidth); //图元操作绘画层的canvas
        this.primitivedrawcan.setAttribute('height', this.viewportHeight);
        this.toolsvas.setAttribute('width', this.viewportWidth);//nuge的canvas
        this.toolsvas.setAttribute('height', this.viewportHeight);
        this.drawCross(this.nix, this.niy, this.canbas.get(0));
    }

    opm3() {//计算窗口比例
        var clientWidth = this.canvas.clientWidth;
        var clientHeight = this.canvas.clientHeight;
        var ratio = clientWidth / clientHeight;
        var scale = ratio > 1 ? 1.0 / clientHeight : 1.0 / clientWidth;
        this.opM3 = mat3.fromValues(scale, 0, 0,
            0, -1 * scale, 0,
            -0.5 * clientWidth * scale, 0.5 * clientHeight * scale, 1);
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


    base64tobin(base64) {
        var text = window.atob(base64);
        var buffer = new ArrayBuffer(text.length);
        var view = new DataView(buffer);
        for (var i = 0, n = text.length; i < n; i++) view.setUint8(i, text.charCodeAt(i));
        return buffer;
    }

    // 翻页
    windowAddMouseWheel(tag) {
        let that = this;
        let delt: any;
        var scrollFunc = function (e) {
            e = e || window.event;
            delt = e.wheelDelta / 120;
            that.P2Cross(delt);
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
        this.conMessage.setActionInfo(new KeyValuePair(actions.clear))
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

        this.horizontalLine = new createjs.Shape();// 横线
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

        this.verticalLine = new createjs.Shape();// 竖线
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
        console.log('update')
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


    //patient2screen
    patient2Mpr: any;
    scrCrossPt: any
    patient2screen(postPoint) {
        // this.postPoint = postPoint;
        // mat4.invert(this.patient2Mpr, this.mpr2Patient);
        // vec4.transformMat4(this.scrCrossPt, this.postPoint, this.patient2Mpr);
        this.scrCrossPt = vec3.fromValues(this.scrCrossPt[0], this.scrCrossPt[1], 1);
        var inaff = mat3.create();
        inaff = mat3.invert(inaff, this.affineMat3);
        vec3.transformMat3(this.scrCrossPt, this.scrCrossPt, inaff);
        var inopm3 = mat3.create();
        inopm3 = mat3.invert(inopm3, this.opM3);
        vec3.transformMat3(this.scrCrossPt, this.scrCrossPt, inopm3);
        //   this.cross(this.scrCrossPt[0], this.scrCrossPt[1], this.canbas.get(0));
    }

    onChangewlww(inval) {
        console.log(inval)
        let wwwlStrArray = inval.split(',')
        let wwwlIntArray = []
        let flag = 'true'
        wwwlStrArray.forEach(element => {''
            try{
                wwwlIntArray.push(Number(element));
            }
            catch(err){
                console.log('cant convert to number');
                flag = 'false'
            }
        });        
        if(typeof(wwwlIntArray[0])=='number' && wwwlIntArray[0] > 0){
            this.ww = wwwlIntArray[0]
        }
        else{
            flag = 'false'
        }
        if(typeof(wwwlIntArray[1])=='number'){
            this.wl = wwwlIntArray[1]
        }
        else{
            flag = 'false'
        }
        this.wwwlReq2.emit([this.ww, this.wl, flag]);
    }

    clearmouse() {
        $('#threebmp').removeClass();
        this.canbas.get(0).onmousedown = null;
    }

    addPanEvent() {
        let that = this;
        $('#threebmp').removeClass().addClass("MoveCursor");
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
        $('#threebmp').removeClass().addClass("ZoomCursor");
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
        let that = this;
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
        }
    }
    addChangeWlEvent() {
        let that = this;
        that.toolsvas.onmousedown = function (e) {
            let ww_factor = 0;
            let wl_factor = 0;
            let preX = e.clientX
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
            }
            that.toolsvas.onmouseup = function (e) {
                that.toolsvas.onmousemove = null;
                that.toolsvas.onmouseup = null;
            }
        }
    }

    ceateAffineTrans(scale, transX, transY) {
        //scaleMatrix3*translateMatrix3先平移再缩放
        var aff3 = mat3.create();
        aff3 = mat3.fromValues(scale, 0, 0,
            0, scale, 0,
            transX, transY, 1);
        return aff3;
    }

    // Start here
    gl: any;
    programInfo: any;
    buffers: any;
    texture: any;
    setupScene() {
        var glCanvas = this.element.nativeElement.querySelector("#demoCanvas");
        this.gl = glCanvas.getContext("webgl");
        if (!this.gl) {
            alert('Unable to initialize WebGL. Your browser or machine may not support it.');
            return;
        }

        // 初始化着色器程序；这是建立顶点等所有照明的地方。
        var shaderProgram = this.initShaderProgram(this.gl, this.glsource.mainVertS, this.glsource.mainFragS);
        // Collect all the info needed to use the shader program.Look up which attribute our shader program is using for aVertexPosition and look up uniform locations.
        this.programInfo = {
            program: shaderProgram,
            attribLocations: {
                vertexPosition: this.gl.getAttribLocation(shaderProgram, 'aVertexPosition'),
                textureCoord: this.gl.getAttribLocation(shaderProgram, "aTextureCoord"),
            },
            uniformLocations: {
                projectionMatrix: this.gl.getUniformLocation(shaderProgram, 'uProjectionMatrix'),
                modelViewMatrix: this.gl.getUniformLocation(shaderProgram, 'uModelViewMatrix'),
                uSampler: this.gl.getUniformLocation(shaderProgram, "uSampler"),
                rescale: this.gl.getUniformLocation(shaderProgram, "rescale"),
                winLevel: this.gl.getUniformLocation(shaderProgram, "windowLevel"),
            },
        };

        this.buffers = this.initBuffers(this.gl);
        this.texture = this.loadTexture(this.gl);
        this.affineMat3 = mat3.create();
        this.affineMat4 = mat4.create();
    }

    // initBuffers Initialize the buffers we'll need. For this demo, we just have one object -- a simple two-dimensional square.
    initBuffers(gl) {
        // Create a buffer for the square's positions.
        var positionBuffer = gl.createBuffer();
        // Select the positionBuffer as the one to apply buffer operations to from here out.
        gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
        // Now create an array of positions for the square.
        var halfWidth = 0.5;
        var halfHeight = 0.5;
        // a quad
        const positions = [
            halfWidth, halfHeight,
            -halfWidth, halfHeight,
            halfWidth, -halfHeight,
            -halfWidth, -halfHeight,
        ];
        // 现在将位置列表传递到WebGL中以构建形状。我们这样做是通过从JavaScript数组中创建一个FLUAT32数组，然后用它来填充当前的缓冲区。
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);
        // 设置纹理坐标缓冲
        var textureCoordBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, textureCoordBuffer);
        var textureCoordinates = [
            1.0, 1.0,
            0.0, 1.0,
            1.0, 0.0,
            0.0, 0.0,
        ];
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(textureCoordinates), gl.STATIC_DRAW);
        return {
            position: positionBuffer,
            textureCoord: textureCoordBuffer,
        };
    }

    loadTexture(gl) {
        var texture = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, texture);
        // 因为图像必须通过因特网下载，所以它们可能需要一段时间，直到它们准备好。
        // 然后在纹理中放置一个像素，这样我们就可以立即使用它。当图像完成下载时，我们将用图像的内容更新纹理。
        var level = 0;
        var internalFormat = gl.RGBA;
        var width = this.imageWidth;
        var height = this.imageHeight;
        var border = 0;
        var format = gl.RGBA;
        var type = gl.UNSIGNED_SHORT_4_4_4_4;
        gl.texImage2D(gl.TEXTURE_2D, level, internalFormat, width, height, border, format, type, this.unts);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
        gl.bindTexture(gl.TEXTURE_2D, null);
        return texture;
    }

    createOrthoProjectMatrix(gl) {
        var width = gl.canvas.clientWidth;
        var height = gl.canvas.clientHeight;
        var viewportSize = 0.5;
        var left = -1 * viewportSize;
        var right = 1 * viewportSize;
        var bottom = -1 * viewportSize;
        var top = 1 * viewportSize;
        var near = 0.1;
        var far = 100.0;
        var ratio = width / height;
        var delt = 0;
        if (ratio > 1) {
            left = left * ratio;
            right = right * ratio;
            delt = (bottom - top) / 2.0;
        }
        else {
            top = (1 / ratio) * top;
            bottom = (1 / ratio) * bottom;
            delt = (right - left) / 2.0;
        }

        var ptLB = vec2.fromValues(left, bottom);
        var ptRT = vec2.fromValues(right, top);
        vec2.transformMat3(ptLB, ptLB, this.affineMat3);
        vec2.transformMat3(ptRT, ptRT, this.affineMat3);

        left = ptLB[0];
        bottom = ptLB[1];
        right = ptRT[0];
        top = ptRT[1];

        const projectionMatrix = mat4.create();
        mat4.ortho(projectionMatrix,
            left,
            right,
            bottom,
            top,
            near,
            far);
        return projectionMatrix;
    }

    createOrthoModelViewMatrix(gl) {
        const eye = vec3.fromValues(0.0, 0.0, 100.0);
        const center = vec3.fromValues(0.0, 0.0, 0.0);
        const up = vec3.fromValues(0.0, 1.0, 0.0);
        const modelViewMatrix = mat4.create();
        mat4.lookAt(modelViewMatrix, eye, center, up);
        return modelViewMatrix;
    }

    // Draw the scene.
    drawScene(gl, programInfo, buffers, texture) {
        gl.viewport(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight);
        gl.clearColor(0.5, 0.5, 0.5, 1.0);  // Clear to black, fully opaque
        gl.clearDepth(1.0);                 // Clear everything
        gl.enable(gl.DEPTH_TEST);           // Enable depth testing
        gl.depthFunc(gl.LEQUAL);            // Near things obscure far things

        // Clear the canvas before we start drawing on it.
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        // 告诉WebGL如何将位置从位置缓冲区拔出到顶点位置属性。
        {
            var numComponents = 2;
            var type = gl.FLOAT;
            var normalize = false;
            var stride = 0;
            var offset = 0;
            gl.bindBuffer(gl.ARRAY_BUFFER, buffers.position);
            gl.vertexAttribPointer(programInfo.attribLocations.vertexPosition, numComponents, type, normalize, stride, offset);
            gl.enableVertexAttribArray(programInfo.attribLocations.vertexPosition);
        }
        // 设置属性-纹理坐标缓冲
        {
            var numComponents = 2;
            var type = gl.FLOAT;
            var normalize = false;
            var stride = 0;
            var offset = 0;
            gl.bindBuffer(gl.ARRAY_BUFFER, buffers.textureCoord);
            gl.vertexAttribPointer(programInfo.attribLocations.textureCoord, numComponents, type, normalize, stride, offset);
            gl.enableVertexAttribArray(programInfo.attribLocations.textureCoord);
        }
        const projectionMatrix = this.createOrthoProjectMatrix(gl);
        const modelViewMatrix = this.createOrthoModelViewMatrix(gl);
        // Set the shader uniforms
        gl.uniformMatrix4fv(programInfo.uniformLocations.projectionMatrix, false, projectionMatrix);
        gl.uniformMatrix4fv(programInfo.uniformLocations.modelViewMatrix, false, modelViewMatrix);

        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, texture);

        gl.uniform1i(programInfo.uniformLocations.uSampler, 0);
        gl.uniform2f(programInfo.uniformLocations.rescale, this.rescaleSlope, this.rescaleIntercept);
        gl.uniform2f(programInfo.uniformLocations.winLevel, this.wl, this.ww);

        {
            var offset = 0;
            var vertexCount = 4;
            gl.drawArrays(gl.TRIANGLE_STRIP, offset, vertexCount);
        }
        gl.bindTexture(gl.TEXTURE_2D, null);
    }

    // 初始化着色器程序，所以WebGL知道如何绘制我们的数据
    initShaderProgram(gl, vsSource, fsSource) {
        var vertexShader = this.loadShader(gl, gl.VERTEX_SHADER, vsSource);
        var fragmentShader = this.loadShader(gl, gl.FRAGMENT_SHADER, fsSource);
        // Create the shader program
        var shaderProgram = gl.createProgram();
        gl.attachShader(shaderProgram, vertexShader);
        gl.attachShader(shaderProgram, fragmentShader);
        gl.linkProgram(shaderProgram);
        // If creating the shader program failed, alert
        if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
            alert('Unable to initialize the shader program: ' + gl.getProgramInfoLog(shaderProgram));
            return null;
        }
        // Tell WebGL to use our program when drawing
        gl.useProgram(shaderProgram);
        return shaderProgram;
    }

    // 创建给定类型的着色器，上传载源并编译它。
    loadShader(gl, type, source) {
        var shader = gl.createShader(type);
        // Send the source to the shader object
        gl.shaderSource(shader, source);
        // Compile the shader program
        gl.compileShader(shader);
        // See if it compiled successfully
        if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
            alert('An error occurred compiling the shaders: ' + gl.getShaderInfoLog(shader));
            gl.deleteShader(shader);
            return null;
        }
        return shader;
    }

    cellUpdate(imageData, crossPoint, graphics = null, wwwl = null) {
        if (imageData != null)
            this.drawCanvasPic(imageData);
        if (crossPoint != null)
            this.cross(crossPoint[0], crossPoint[1], 1);
        this.updateGraphics(graphics);
        this.updateWWWL(wwwl);
    }

    updateGraphics(graphics) {
        let g = [this.tag, graphics]
        this.conMessage.setGraphics(g);
    }

    drawCanvasPic(imageData) {
        let img1 = new Image();
        let base64Header = "data:image/jpeg;base64,";
        let imgData1 = base64Header + imageData;
        img1.src = imgData1;
        let that = this;
        img1.onload = function () {
            let ctx1 = that.canvas.getContext("2d");
            ctx1.clearRect(0, 0, that.canvas.width, that.canvas.height);
            ctx1.drawImage(img1, 0, 0, that.canvas.width, that.canvas.height);
        }
    }
    updateWWWL(wwwl){
        if(wwwl != null && wwwl != undefined && Array.isArray(wwwl) == true){
            this.viewWW = Math.round(wwwl[0])
            this.viewWL = Math.round(wwwl[1])
        }
    }

}
