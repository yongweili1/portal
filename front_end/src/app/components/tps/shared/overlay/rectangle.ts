import { Overlay } from '../overlay/overlay'

export class Rectangle extends Overlay {

    constructor(stage) {
        super(stage, 'rectangle');
    }

    update(){
        super.update();
        this.overlayStage.clear();
        this.graphics.clear();
        this.drawControlPoints();
        this.graphics.beginStroke("white")
                     .setStrokeStyle(1, "round")
                     .rect(this._startPoint.x, this._startPoint.y, this._endPoint.x - this._startPoint.x, this._endPoint.y - this._startPoint.y);
        this.overlayStage.update();
    }
}
