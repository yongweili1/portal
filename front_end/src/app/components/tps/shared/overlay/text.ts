import { Point } from '../tools/point';
import { Hitbar } from '../overlay/hitbar'

declare var createjs: any;

export class Text extends createjs.Text {
    type: string;
    overlayStage: any;
    protected _cp: Point;
    host: any;
    index: string;
    protected _tempPoint: Point;

    constructor(stage, host, point, text) {
        super(text, "20px Arial", "#F00");
        this.overlayStage = stage;
        this.host = host;
        this.type = 'text';
        this._cp = point;
        this._tempPoint = new Point(0, 0);
        this.addEventListener("mousedown", this.handleMouseDown.bind(this));
        this.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.addEventListener("dblclick", this.handleDbClick.bind(this));
        this.addEventListener("pressup", this.handlePressUp.bind(this));
        this.addEventListener("mouseover", this.handleMouseOver.bind(this));
        this.overlayStage.addChild(this);
    }

    setCp(p) {
        this._cp = p;
    }

    update(){
        this.overlayStage.clear();
        this.graphics.clear();
        this.graphics.beginStroke("yellow")
                     .setStrokeStyle(1, "round")
                     .rect(this._center.x - 2, this._center.y - 2, 4, 4);
        
        let hit = new Hitbar();
        hit.graphics.rect(this._center.x - 2, this._center.y - 2, 4, 4);
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
    }
}
