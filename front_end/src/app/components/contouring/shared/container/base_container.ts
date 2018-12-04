import { Point } from '../tools/point';
import { RoiModel } from '../model/roi.model';
import { ShapeTypeEnum } from '../../../../shared/models/enums';
declare var createjs: any;

export class BaseContainer extends createjs.Container {
    uid: string = null;
    isPaint = false;
    isMousedown = false;
    type: ShapeTypeEnum;
    overlayStage: any;
    protected _tempPoint: Point;
    cps: Array<Point>;

    // -1: no mouse button down
    // 0: left mouse button down
    // 1: middle mouse button down
    // 2: right mouse button down
    currentPressDownBtn: number;

    protected _roiConfig: RoiModel;
    protected _contourInfo: any;

    constructor(stage, type: ShapeTypeEnum) {
        super();
        this.overlayStage = stage;
        this.type = type;
        this._tempPoint = new Point(0, 0);
        this.addEventListener('mousedown', this.handleMouseDown.bind(this));
        this.addEventListener('pressmove', this.handlePressMove.bind(this));
        this.addEventListener('dblclick', this.handleDbClick.bind(this));
        this.addEventListener('pressup', this.handlePressUp.bind(this));
        this.overlayStage.addChild(this);
        this.initCps();
    }

    public get roiConfig() {
        return this._roiConfig;
    }

    public set roiConfig(roiConfig: RoiModel) {
        this._roiConfig = roiConfig;
    }

    public get contourInfo() {
        return this._contourInfo;
    }

    public set contourInfo(contourInfo: any) {
        this._contourInfo = contourInfo;
    }

    public setRoi(roi) {
        this.roiConfig = roi;
    }

    initCps(count: number = -1) {
        this.cps = new Array();
        for (let index = 0; index < count; index++) {
            this.cps.push(new Point(0, 0));
        }
    }

    updateCp(index: number, x, y) {
        if (index < this.cps.length) {
            this.cps[index].update(x, y);
        }
    }

    update() {
    }

    handleMouseDown(evt) {
        console.log('[base_container]handle MouseDown');
        this._tempPoint = new Point(evt.stageX, evt.stageY);
        this.isMousedown = true;
        this.currentPressDownBtn = evt.button;
    }
    handleMouseUp(e) {
        this.isMousedown = false;
        this.currentPressDownBtn = -1;
    }
    handlePressMove(evt) {
        console.log('[base_container]handle PressMove');
        this.update();
    }
    handlePressUp(evt) {
        console.log('[base_container]handle PressUp');
        this._tempPoint = new Point(0, 0);
    }
    handleDbClick(evt) {
        console.log('[base_container]handle DbClick');
    }
}
