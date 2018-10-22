import { Overlay } from '../overlay/overlay'
import { Hitbar } from '../overlay/hitbar'

export class Line extends Overlay {
    type: string = 'line';

    constructor(stage, default_color='white') {
        super(stage, default_color);
    }

    update() {
        super.update();
        this.overlayStage.clear();
        this.graphics.clear();
        this.graphics.beginStroke(this._color)
                     .moveTo(this._startPoint.x, this._startPoint.y)
                     .lineTo(this._endPoint.x, this._endPoint.y);
        let hit = new Hitbar();
        hit.graphics.moveTo(this._startPoint.x, this._startPoint.y)
                    .lineTo(this._endPoint.x, this._endPoint.y);
        this.hitArea = hit;
        this.overlayStage.update();
    }
}
