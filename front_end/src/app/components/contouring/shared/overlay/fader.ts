import { Point } from '../tools/point';

declare var createjs: any;

export class Fader extends createjs.Shape {
    type: string = 'fader';
    overlayStage: any;
    radius: number;
    center: Point;
    protected _color: string;
    cps: Array<Point>;

    constructor(stage) {
        super();
        this.overlayStage = stage;
        this.cps = new Array();
        this.radius = 20;
        this._color = 'yellow';
        this.addEventListener("mouseover", this.handleMouseOver.bind(this));
        this.addEventListener("mouseout", this.handleMouseOut.bind(this));
    }

    setRadius(value:number) {
        this.radius = value;
    }

    setCenter(center) {
        this.center = center;
        this.generateCps();
    }

    update(center) {
        this.setCenter(center);
        this.overlayStage.clear();
        this.graphics.clear();
        // this.graphics.beginStroke(this._color).drawCircle(center.x, center.y, this.radius);
        for (let index = 1; index < this.cps.length; index++) {
            const start = this.cps[index - 1];
            const end = this.cps[index];
            this.graphics.beginStroke(this._color).moveTo(start.x, start.y).lineTo(end.x, end.y);
        }
        // close the circle
        this.graphics.beginStroke(this._color).moveTo(this.cps[this.cps.length - 1].x, this.cps[this.cps.length - 1].y).lineTo(this.cps[0].x, this.cps[0].y);
        this.overlayStage.update();
    }

    generateCps() {
        this.cps = new Array();
        for (let i = 0; i < 2 * Math.PI; i += 0.3) {
            let x = this.radius * Math.cos(i) + this.center.x;
            let y = this.radius * Math.sin(i) + this.center.y;

            this.cps.push(new Point(x, y))
        }
    }

    handleMouseOver(evt) {
        console.log('handle MouseOver', evt.currentTarget.type)
    }
    handleMouseOut(evt) {
        console.log('handle MouseOut', evt.currentTarget.type)
    }
}
