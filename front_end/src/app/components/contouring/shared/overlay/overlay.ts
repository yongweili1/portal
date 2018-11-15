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
        this.addEventListener("mouseover", this.handleMouseOver.bind(this));
        this.addEventListener("mouseout", this.handleMouseOut.bind(this));
    }

    setStartPoint(p) {
        this._startPoint = p;
    }

    setEndPoint(p) {
        this._endPoint = p;
    }

    handleMouseOver(evt) {
        console.log(evt.currentTarget.type + ': handle MouseOver')
        this._is_hover = true;
    }
    handleMouseOut(evt) {
        console.log(evt.currentTarget.type + ': handle MouseOut')
        this._is_hover = false;
    }
    public set color(color) {
        this._color = color;
    }
}
