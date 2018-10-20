import { Point } from '../tools/point'
declare var createjs: any;

export class Overlay extends createjs.Shape {
    overlayStage: any;
    protected _startPoint: Point;
    protected _endPoint: Point;
    protected _is_hover: boolean;
    protected _color: string;

    constructor(stage) {
        super();
        this.overlayStage = stage;
        this._is_hover = false;
        this._color = 'white';
        this.addEventListener("mouseover", this.handleMouseOver.bind(this));
        this.addEventListener("mouseout", this.handleMouseOut.bind(this));
        this.overlayStage.addChild(this);
    }

    setStartPoint(p) {
        this._startPoint = p;
    }

    setEndPoint(p) {
        this._endPoint = p;
    }

    update(){
    }

    handleMouseOver(evt) {
        this._is_hover = true;
        this._color = 'red';
        this.update()
    }
    handleMouseOut(evt) {
        this._is_hover = false;
        this._color = 'white';
        this.update()
    }
}
