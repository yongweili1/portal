import { Point } from '../tools/point'
declare var createjs: any;

export class Overlay extends createjs.Shape {
    overlayStage: any;
    protected _startPoint: Point;
    protected _endPoint: Point;
    protected _is_hover: boolean;
    protected _default_color: string;
    protected _color: string;

    constructor(stage, default_color='white') {
        super();
        this.overlayStage = stage;
        this._is_hover = false;
        this._default_color = default_color;
        this._color = this._default_color;
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

    handleMouseOver(evt) {
        console.log(evt.currentTarget.type + ': handle MouseOver')
        this._is_hover = true;
        this._color = 'red';
    }
    handleMouseOut(evt) {
        console.log(evt.currentTarget.type + ': handle MouseOut')
        this._is_hover = false;
        this._color = this._default_color;
    }
}
