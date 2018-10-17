import { Overlay } from '../overlay/overlay'
import { ControlPoint } from '../overlay/controlpoint'
import { Point } from '../tools/point'
import { Hitbar } from '../overlay/hitbar'

export class Rectangle extends Overlay {
    constructor(stage) {
        super(stage, 'rectangle');
        this._cps = new Array(8);
    }

    setStartPoint(p) {
        super.setStartPoint(p)
        this.setCp(0, p)
    }

    setEndPoint(p) {
        super.setEndPoint(p)
        
        this.setCp(1, new Point(this._startPoint.x + (this._endPoint.x - this._startPoint.x) / 2, this._startPoint.y))
        this.setCp(2, new Point(this._endPoint.x, this._startPoint.y))
        this.setCp(3, new Point(this._startPoint.x, this._startPoint.y + (this._endPoint.y - this._startPoint.y) / 2))
        this.setCp(4, new Point(this._endPoint.x, this._startPoint.y + (this._endPoint.y - this._startPoint.y) / 2))
        this.setCp(5, new Point(this._startPoint.x, this._endPoint.y))
        this.setCp(6, new Point(this._startPoint.x + (this._endPoint.x - this._startPoint.x) / 2, this._endPoint.y))
        this.setCp(7, p)
    }

    setCp(index, p) {
        if (this._cps[index] == null) {
            this._cps[index] = new ControlPoint(this.overlayStage, this, p, index)
        } else {
            this._cps[index].setCenter(p)
        }
    }

    updateCps(delta_x, delta_y) {
        this._cps.forEach(cp => {
            cp.x += delta_x;
            cp.y += delta_y;
        });
    }

    update(){
        super.update();
        this.overlayStage.clear();
        this.graphics.clear();
        this.drawControlPoints();
        let width = this._endPoint.x - this._startPoint.x
        let height = this._endPoint.y - this._startPoint.y
        this.graphics.beginStroke("white").rect(this._startPoint.x, this._startPoint.y, width, height);
        let hit = new Hitbar();
        hit.graphics.rect(this._startPoint.x, this._startPoint.y, width, height);
        this.hitArea = hit;
        this.overlayStage.update();
    }

    updateCp(index, delta_x, delta_y) {
        if (index == 0) {
            this._startPoint.x += delta_x;
            this._startPoint.y += delta_y;
        } else if (index == 1) {
            this._startPoint.y += delta_y;
        } else if (index == 2) {
            this._startPoint.y += delta_y;
            this._endPoint.x += delta_x;
        } else if (index == 3) {
            this._startPoint.x += delta_x;
        } else if (index == 4) {
            this._endPoint.x += delta_x;
        } else if (index == 5) {
            this._startPoint.x += delta_x;
            this._endPoint.y += delta_y;
        } else if (index == 6) {
            this._endPoint.y += delta_y;
        } else if (index == 7) {
            this._endPoint.x += delta_x;
            this._endPoint.y += delta_y;
        } else {

        }
        this.setStartPoint(this._startPoint)
        this.setEndPoint(this._endPoint)
        this.update()
    }
}
