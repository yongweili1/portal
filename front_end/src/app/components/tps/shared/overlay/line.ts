import { Overlay } from '../overlay/overlay'

export class Line extends Overlay {

    constructor(stage) {
        super(stage, 'line');
    }

    setCps() {
        this.clearCps()
        this._cps.push(this._startPoint)
        this._cps.push(this._endPoint)
    }

    update(){
        super.update();
        this.overlayStage.clear();
        this.graphics.clear();
        this.setCps();
        this.drawControlPoints();
        this.graphics.beginStroke("white")
                     .setStrokeStyle(1, "round")
                     .moveTo(this._startPoint.x, this._startPoint.y)
                     .lineTo(this._endPoint.x, this._endPoint.y);
        this.overlayStage.update();
    }
}
