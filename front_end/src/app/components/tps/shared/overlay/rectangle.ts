import { Overlay } from '../overlay/overlay'
import { Hitbar } from '../overlay/hitbar'

export class Rectangle extends Overlay {
    type: string = 'rectangle';

    constructor(stage) {
        super(stage);
    }

    update(start, end) {
        this.overlayStage.clear();
        this.graphics.clear();
        let width = end.x - start.x
        let height = end.y - start.y
        this.graphics.beginStroke(this._color).rect(start.x, start.y, width, height);
        let hit = new Hitbar();
        hit.graphics.rect(start.x, start.y, width, height);
        this.hitArea = hit;
        this.overlayStage.update();
    }
}
