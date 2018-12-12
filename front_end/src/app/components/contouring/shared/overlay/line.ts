import { Overlay } from '../overlay/overlay';
import { Hitbar } from '../overlay/hitbar';
import { ShapeTypeEnum } from '../../../../shared/models/enums';

export class Line extends Overlay {
    type = ShapeTypeEnum.line;

    constructor(stage) {
        super(stage);
    }

    update(start, end) {
        this.overlayStage.clear();
        this.graphics.clear();

        this.graphics.beginStroke(this._color)
                     .moveTo(start.x, start.y)
                     .lineTo(end.x, end.y);
        const hit = new Hitbar();
        hit.graphics.moveTo(start.x, start.y)
                    .lineTo(end.x, end.y);
        this.hitArea = hit;
        this.overlayStage.update();
    }
}
