import { Overlay } from '../overlay/overlay'

export class Line extends Overlay {

    constructor(stage) {
        super(stage, 'line');
    }

    update(){
        super.update();
        this.overlayStage.clear();
        this.graphics.clear();
        this.drawControlPoints();
        this.graphics.beginStroke("white")
                     .setStrokeStyle(1, "round")
                     .moveTo(this._startPoint.x, this._startPoint.y)
                     .lineTo(this._endPoint.x, this._endPoint.y);
        this.overlayStage.update();
    }
}
