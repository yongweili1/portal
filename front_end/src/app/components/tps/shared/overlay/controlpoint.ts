import { Point } from '../tools/point';
import { Hitbar } from '../overlay/hitbar'

declare var createjs: any;

export class ControlPoint extends createjs.Shape {
    type: string;
    overlayStage: any;
    protected _center: Point;
    host: any;
    index: string;
    protected _tempPoint: Point;
    protected _is_hover: boolean;
    protected _color: string;

    constructor(stage, host, point, index) {
        super();
        this.overlayStage = stage;
        this.host = host;
        this.type = 'controlpoint';
        this._center = point;
        this.index = index;
        this._tempPoint = new Point(0, 0);
        this._is_hover = false;
        this._color = 'yellow';
        this.addEventListener("mousedown", this.handleMouseDown.bind(this));
        this.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.addEventListener("dblclick", this.handleDbClick.bind(this));
        this.addEventListener("pressup", this.handlePressUp.bind(this));
        this.addEventListener("mouseover", this.handleMouseOver.bind(this));
        this.addEventListener("mouseout", this.handleMouseOut.bind(this));
        this.overlayStage.addChild(this);
    }

    setCenter(p) {
        this._center = p;
    }

    update(){
        this.overlayStage.clear();
        this.graphics.clear();
        this.graphics.beginStroke(this._color)
                     .rect(this._center.x - 2, this._center.y - 2, 4, 4);
        let hit = new Hitbar();
        hit.graphics.rect(this._center.x - 3, this._center.y - 3, 6, 6);
        this.hitArea = hit;
        this.overlayStage.update();
    }

    handleMouseDown(evt) {
        console.log('handleMouseDown')
        this._tempPoint = new Point(0, 0);
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;
    }
    handlePressMove(evt) {
        let delta_x = evt.stageX - this._tempPoint.x;
        let delta_y = evt.stageY - this._tempPoint.y;
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;

        console.log('cp:', this.index)
        this.host.updateCp(this.index, delta_x, delta_y)
    }
    handlePressUp(evt) {
        console.log('handlePressUp')
        this._tempPoint = new Point(0, 0);
    }
    handleDbClick(evt) {
        console.log('handleDbClick')
        this.curTarget = evt.currentTarget;
        this.overlayStage.removeChild(this.curTarget);
        this.overlayStage.clear();
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
