import { Point } from '../tools/point';
import { Hitbar } from '../overlay/hitbar'

declare var createjs: any;

export class ControlPoint extends createjs.Shape {
    type: string;
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
        this.addEventListener("mouseover", this.handleMouseOver.bind(this));
        this.addEventListener("mouseout", this.handleMouseOut.bind(this));
        this.overlayStage.addChild(this);
    }

    setCenter(point) {
        this._center = point
    }
    getCenter() {
        return this._center
    }
    updateCenter(delta_x=null, delta_y=null) {
        if (delta_x != null) {
            this._center.x += delta_x
        }
        if (delta_y != null) {
            this._center.y += delta_y
        }
    }

    update() {
        this.overlayStage.clear();
        this.graphics.clear();
        this.graphics.beginStroke(this._color)
                     .rect(this._center.x - 2, this._center.y - 2, 4, 4);
        let hit = new Hitbar();
        hit.graphics.rect(this._center.x - 3, this._center.y - 3, 6, 6);
        this.hitArea = hit;
        this.overlayStage.update();
    }

    handleMouseOver(evt) {
        console.log('handleMouseOver on', evt.currentTarget.type)
        this._is_hover = true;
        this._color = 'red';
        this.update()
    }
    handleMouseOut(evt) {
        console.log('handleMouseOut on', evt.currentTarget.type)
        this._is_hover = false;
        this._color = 'yellow';
        this.update()
    }
}
