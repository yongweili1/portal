import { Overlay } from '../overlay/overlay'
import { Hitbar } from '../overlay/hitbar'

export class Circle extends Overlay {

    constructor(stage) {
        super(stage);
    }

    update(){
        super.update();
        this.overlayStage.clear();
        this.graphics.clear();

        let x = (this._endPoint.x + this._startPoint.x) / 2;
        let y = (this._endPoint.y + this._startPoint.y) / 2;
        let x_side = this._endPoint.x - this._startPoint.x
        let y_side = this._endPoint.y - this._startPoint.y
        let radio = Math.sqrt(x_side * x_side + y_side * y_side) / 2;
        this.graphics.beginStroke(this._color).drawCircle(x, y, radio);
        let hit = new Hitbar();
        hit.graphics.drawCircle(x, y, radio);
        this.hitArea = hit;
        this.overlayStage.update();
    }
}
