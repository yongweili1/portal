import { Point } from '../tools/point';
import { Overlay } from './overlay';
import { ShapeTypeEnum } from '../../../../shared/models/enums';

export class Fader extends Overlay {
    type = ShapeTypeEnum.fader;
    radius: number;
    center: Point;
    cps: Array<Point>;

    constructor(stage) {
        super(stage);
        this.cps = new Array();
        this.radius = 20;
    }

    setRadius(value: number) {
        this.radius = value;
    }

    setCenter(center) {
        this.center = center;
        this.generateCps();
    }

    update(center, isFill) {
        this.setCenter(center);
        this.overlayStage.clear();
        this.graphics.clear();
        this.graphics.setStrokeStyle(1);
        if (isFill) {
            this.fillCommand = this.graphics.beginFill(this._color + this._alpha).command;
            this.strokeCommand = this.graphics.beginStroke(this._color).command;
        }
        this.graphics.beginStroke(this._color);
        this.graphics.moveTo(this.cps[0].x, this.cps[0].y);
        for (let index = 1; index < this.cps.length; index++) {
            const pt = this.cps[index];
            this.graphics.lineTo(pt.x, pt.y);
        }
        // close the circle
        this.graphics.lineTo(this.cps[0].x, this.cps[0].y);
        this.overlayStage.update();
    }

    generateCps() {
        this.cps = new Array();
        for (let i = 0; i < 2 * Math.PI; i += 0.3) {
            const x = this.radius * Math.cos(i) + this.center.x;
            const y = this.radius * Math.sin(i) + this.center.y;

            this.cps.push(new Point(x, y));
        }
    }
}
