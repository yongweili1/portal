import { Point } from '../tools/point';
import { Hitbar } from '../overlay/hitbar';
import { ShapeTypeEnum } from '../../../../shared/models/enums';

declare var createjs: any;
declare var showcps: any;

export class ControlPoint extends createjs.Shape {
    type = ShapeTypeEnum.controlpoint;
    overlayStage: any;
    host: any;
    index: string;
    protected _center: Point;
    protected _tempPoint: Point;
    protected _is_hover: boolean;
    protected _color: string;

    constructor(stage) {
        super();
        this.overlayStage = stage;
        this._tempPoint = new Point(0, 0);
        this._is_hover = false;
        this._color = 'yellow';
        this.addEventListener('mouseover', this.handleMouseOver.bind(this));
        this.addEventListener('mouseout', this.handleMouseOut.bind(this));
        this.overlayStage.addChild(this);
    }

    update(point) {
        this._center = point;
        if (showcps) {
            this.overlayStage.clear();
            this.graphics.clear();
            this.graphics.beginStroke(this._color)
                .rect(point.x - 2, point.y - 2, 4, 4);
            const hit = new Hitbar();
            hit.graphics.rect(point.x - 3, point.y - 3, 6, 6);
            this.hitArea = hit;
            this.overlayStage.update();
        }
    }

    handleMouseOver(evt) {
        console.log('handle MouseOver', evt.currentTarget.type);
        this._is_hover = true;
        this._color = 'red';
        this.update(this._center);
    }
    handleMouseOut(evt) {
        console.log('handle MouseOut', evt.currentTarget.type);
        this._is_hover = false;
        this._color = 'yellow';
        this.update(this._center);
    }
}
