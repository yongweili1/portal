import { Overlay } from '../overlay/overlay';
import { Hitbar } from '../overlay/hitbar';
import { ShapeTypeEnum } from '../../../../shared/models/enums';

export class Rectangle extends Overlay {
    type = ShapeTypeEnum.rectangle;

    constructor(stage) {
        super(stage);
    }

    update(start, end, isFill) {
        this.overlayStage.clear();
        this.graphics.clear();
        const width = end.x - start.x;
        const height = end.y - start.y;
        this.graphics.beginStroke(this._color);
        if (isFill) {
            this.graphics.beginFill(this._color + this._alpha);
        } else {
            const hit = new Hitbar();
            hit.graphics.rect(start.x, start.y, width, height);
            this.hitArea = hit;
            this.overlayStage.update();
        }
        this.graphics.rect(start.x, start.y, width, height);
    }
}
