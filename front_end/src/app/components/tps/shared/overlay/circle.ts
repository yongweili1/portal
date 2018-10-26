import { Overlay } from '../overlay/overlay'
import { Hitbar } from '../overlay/hitbar'

export class Circle extends Overlay {
    type: string = 'circle';

    constructor(stage) {
        super(stage);
    }

    update(start, end){
        this.overlayStage.clear();
        this.graphics.clear();

        let x = (end.x + start.x) / 2;
        let y = (end.y + start.y) / 2;
        let x_side = end.x - start.x
        let y_side = end.y - start.y
        let radio = Math.sqrt(x_side * x_side + y_side * y_side) / 2;
        this.graphics.beginStroke(this._color).drawCircle(x, y, radio);
        let hit = new Hitbar();
        hit.graphics.drawCircle(x, y, radio);
        this.hitArea = hit;
        this.overlayStage.update();
    }
}
