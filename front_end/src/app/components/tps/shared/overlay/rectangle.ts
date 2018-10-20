import { Overlay } from '../overlay/overlay'
import { Hitbar } from '../overlay/hitbar'

export class Rectangle extends Overlay {
    constructor(stage) {
        super(stage);
    }

    update(){
        super.update();
        this.overlayStage.clear();
        this.graphics.clear();
        let width = this._endPoint.x - this._startPoint.x
        let height = this._endPoint.y - this._startPoint.y
        this.graphics.beginStroke(this._color).rect(this._startPoint.x, this._startPoint.y, width, height);
        let hit = new Hitbar();
        hit.graphics.rect(this._startPoint.x, this._startPoint.y, width, height);
        this.hitArea = hit;
        this.overlayStage.update();
    }
}
