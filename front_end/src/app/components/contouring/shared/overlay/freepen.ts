import { Overlay } from '../overlay/overlay';
import { Hitbar } from '../overlay/hitbar';
import { Point } from '../tools/point';
import { ShapeTypeEnum } from '../../../../shared/models/enums';

export class Freepen extends Overlay {
    type = ShapeTypeEnum.freepen;
    protected _cps: Array<Point>;

    constructor(stage) {
        super(stage);
        this._cps = new Array();
    }

    update(cps) {
        this.overlayStage.clear();
        this.graphics.clear();

        for (let index = 1; index < cps.length; index++) {
            const start = cps[index - 1];
            const end = cps[index];
            this.graphics.beginStroke(this._color).moveTo(start.x, start.y).lineTo(end.x, end.y);
            const hit = new Hitbar();
            hit.graphics.moveTo(start.x, start.y).lineTo(end.x, end.y);
        }

        this.overlayStage.update();
    }
}
