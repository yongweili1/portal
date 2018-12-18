import { Overlay } from '../overlay/overlay';
import { Hitbar } from '../overlay/hitbar';
import { ShapeTypeEnum } from '../../../../shared/models/enums';

export class Circle extends Overlay {
    type = ShapeTypeEnum.circle;

    constructor(stage) {
        super(stage);
    }

    update(start, end, isFill) {
        this.overlayStage.clear();
        this.graphics.clear();

        const x = (end.x + start.x) / 2;
        const y = (end.y + start.y) / 2;
        const x_side = end.x - start.x;
        const y_side = end.y - start.y;
        const radio = Math.sqrt(x_side * x_side + y_side * y_side) / 2;
        this.graphics.beginStroke(this._color);
        if (isFill) {
            this.graphics.beginFill(this._color + this._alpha);
        } else {
            const hit = new Hitbar();
            hit.graphics.drawCircle(x, y, radio);
            this.hitArea = hit;
        }
        this.graphics.drawCircle(x, y, radio);
        this.overlayStage.update();
    }
}
