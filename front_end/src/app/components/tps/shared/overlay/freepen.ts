import { Overlay } from '../overlay/overlay'
import { Hitbar } from '../overlay/hitbar'
import { Point } from '../tools/point';

export class Freepen extends Overlay {
    type: string = 'freepen';
    protected _cps: Array<Point>;

    constructor(stage, default_color='white') {
        super(stage, default_color);
        this._cps = new Array();
    }

    appendCp(point: Point) {
        this._cps.push(point)
    }

    getCp(index=-1) {
        if (index == -1) {
            return this._cps;
        } else {
            return this._cps[index];
        }
    }
    updateCps(cps) {
        this._cps = new Array()
        cps.forEach(cp => {
            this._cps.push(cp.getCenter())
        }); 
    }
    moveCps(delta_x, delta_y) {
        this._cps.forEach(cp => {
            cp.x += delta_x;
            cp.y += delta_y;
        });
    }

    update() {
        super.update();
        this.overlayStage.clear();
        this.graphics.clear();
        
        for (let index = 1; index < this._cps.length; index++) {
            const start = this._cps[index - 1];
            const end = this._cps[index];
            this.graphics.beginStroke(this._color).moveTo(start.x, start.y).lineTo(end.x, end.y);
            const hit = new Hitbar();
            hit.graphics.moveTo(start.x, start.y).lineTo(end.x, end.y);
        }

        this.overlayStage.update();
    }
}
