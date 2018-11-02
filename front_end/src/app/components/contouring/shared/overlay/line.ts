import { Overlay } from '../overlay/overlay'
import { Hitbar } from '../overlay/hitbar'

export class Line extends Overlay {
    type: string = 'line';

    constructor(stage, default_color='white') {
        super(stage, default_color);
    }

    update(start, end) {
        this.overlayStage.clear();
        this.graphics.clear();
        
        this.graphics.beginStroke(this._color)
                     .moveTo(start.x, start.y)
                     .lineTo(end.x, end.y);
        let hit = new Hitbar();
        hit.graphics.moveTo(start.x, start.y)
                    .lineTo(end.x, end.y);
        this.hitArea = hit;
        this.overlayStage.update();
    }
}
