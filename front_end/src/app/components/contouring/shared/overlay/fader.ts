import { Point } from '../tools/point';

declare var createjs: any;

export class Fader extends createjs.Shape {
    type: string = 'fader';
    overlayStage: any;
    radius: number;
    center: Point;
    protected _color: string;
    disable: boolean;

    constructor(stage) {
        super();
        this.overlayStage = stage;
        this.radius = 20;
        this.disable = false;
        this._color = 'yellow';
        this.addEventListener("mouseover", this.handleMouseOver.bind(this));
        this.addEventListener("mouseout", this.handleMouseOut.bind(this));
        this.overlayStage.addChild(this);
    }

    setRadius(value:number) {
        this.radius = value;
    }

    update(center) {
        this.center = center;
        this.overlayStage.clear();
        this.graphics.clear();
        this.graphics.beginStroke(this._color).drawCircle(center.x, center.y, this.radius);
        this.overlayStage.update();
    }

    handleMouseOver(evt) {
        console.log('handle MouseOver', evt.currentTarget.type)
    }
    handleMouseOut(evt) {
        console.log('handle MouseOut', evt.currentTarget.type)
    }
}
