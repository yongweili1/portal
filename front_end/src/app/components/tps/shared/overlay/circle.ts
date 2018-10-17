import { Overlay } from '../overlay/overlay'
import { ControlPoint } from '../overlay/controlpoint'
import { Hitbar } from '../overlay/hitbar'

export class Circle extends Overlay {

    constructor(stage) {
        super(stage, 'circle');
        this._cps = new Array(2);
    }

    setStartPoint(p) {
        super.setStartPoint(p)
        this.setCp(0, p)
    }

    setEndPoint(p) {
        super.setEndPoint(p)
        this.setCp(1, p)
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

        let x = (this._endPoint.x + this._startPoint.x) / 2;
        let y = (this._endPoint.y + this._startPoint.y) / 2;
        let x_side = this._endPoint.x - this._startPoint.x
        let y_side = this._endPoint.y - this._startPoint.y
        let radio = Math.sqrt(x_side * x_side + y_side * y_side) / 2;
        this.graphics.beginStroke("white").drawCircle(x, y, radio);
        let hit = new Hitbar();
        hit.graphics.drawCircle(x, y, radio);
        this.hitArea = hit;
        this.overlayStage.update();
    }

    updateCp(index, delta_x, delta_y) {
        if (index == 0) {
            console.log('update cp0')
            this._startPoint.x += delta_x;
            this._startPoint.y += delta_y;
            this.setCp(index, this._startPoint)
        } else if (index == 1) {
            console.log('update cp1')
            this._endPoint.x += delta_x;
            this._endPoint.y += delta_y;
            this.setCp(index, this._endPoint)
        } else {

        }
        this.update()
    }
}
