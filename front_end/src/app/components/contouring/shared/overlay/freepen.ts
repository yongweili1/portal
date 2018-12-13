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

    update(cps, isFill) {
        if (cps === undefined || cps.length === 0) {
            return;
        }
        this.overlayStage.clear();
        this.graphics.clear();
        this.graphics.setStrokeStyle(1);
        if (isFill) {
            this.fillCommand = this.graphics.beginFill(this._color + this._alpha).command;
            this.strokeCommand = this.graphics.beginStroke(this._color).command;
        }

        this.graphics.beginStroke(this._color);
        this.graphics.moveTo(cps[0].x, cps[0].y);
        for (let index = 1; index < cps.length; index++) {
            const pt = cps[index];
            this.graphics.lineTo(pt.x, pt.y);
        }

        this.overlayStage.update();
    }
}
