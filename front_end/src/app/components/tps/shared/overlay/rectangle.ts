import { Overlay } from '../overlay/overlay'
import { Point } from '../tools/point';

export class Rectangle extends Overlay {

    constructor(stage) {
        super(stage, 'rectangle');
    }

    setCps() {
        this.clearCps()
        this._cps.push(this._startPoint)
        this._cps.push(new Point(this._startPoint.x, this._endPoint.y))
        this._cps.push(new Point(this._endPoint.x, this._startPoint.y))
        this._cps.push(this._endPoint)
        this._cps.push(new Point(this._startPoint.x + (this._endPoint.x - this._startPoint.x) / 2, this._startPoint.y))
        this._cps.push(new Point(this._startPoint.x + (this._endPoint.x - this._startPoint.x) / 2, this._endPoint.y))
        this._cps.push(new Point(this._startPoint.x, this._startPoint.y + (this._endPoint.y - this._startPoint.y) / 2))
        this._cps.push(new Point(this._endPoint.x, this._startPoint.y + (this._endPoint.y - this._startPoint.y) / 2))
    }

    update(){
        super.update();
        this.overlayStage.clear();
        this.graphics.clear();
        this.setCps();
        this.drawControlPoints();
        let width = this._endPoint.x - this._startPoint.x
        let height = this._endPoint.y - this._startPoint.y
        this.graphics.beginStroke("white")
                     .setStrokeStyle(1, "round")
                     .rect(this._startPoint.x, this._startPoint.y, width, height);
        this.overlayStage.update();
    }
}
