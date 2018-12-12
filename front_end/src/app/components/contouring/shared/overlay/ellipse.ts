import { Overlay } from '../overlay/overlay';
import { ShapeTypeEnum } from '../../../../shared/models/enums';

export class Ellipse extends Overlay {
    type = ShapeTypeEnum.ellipse;

    constructor(stage) {
        super(stage);
    }

    update() {
        super.update();
        this.overlayStage.clear();
        this.graphics.clear();

        this.drawControlPoints();
        const x = (this._endPoint.x - this._startPoint.x) / 2;
        const y = (this._endPoint.y - this._startPoint.y) / 2;
        const radio = Math.sqrt((this._endPoint.x - this._startPoint.x) * (this._endPoint.x - this._startPoint.x)
            + (this._endPoint.y - this._startPoint.y) * (this._endPoint.y - this._startPoint.y));
        this.graphics.beginStroke('#2196F3')
            .setStrokeStyle(1, 'round')
            .drawEllipse(x, y, radio);
        this.overlayStage.update();
    }
}
