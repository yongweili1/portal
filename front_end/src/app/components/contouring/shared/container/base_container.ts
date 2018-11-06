import { Point } from '../tools/point'
declare var createjs: any;

export class BaseContainer extends createjs.Container {
    isPaint: boolean = false;
    isMousedown: boolean = true;
    type: string;
    overlayStage: any;
    protected _tempPoint: Point;
    cps: Array<Point>;

    constructor(stage, type) {
        super();
        this.overlayStage = stage;
        this.type = type;
        this._tempPoint = new Point(0, 0);
        this.addEventListener("mousedown", this.handleMouseDown.bind(this));
        this.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.addEventListener("dblclick", this.handleDbClick.bind(this));
        this.addEventListener("pressup", this.handlePressUp.bind(this));
        this.overlayStage.addChild(this);
        this.initCps()
    }

    initCps(count:number=-1) {
        this.cps = new Array()
        for (let index = 0; index < count; index++) {
            this.cps.push(new Point(0, 0))
        }
    }

    updateCp(index:number, x, y) {
        if (index < this.cps.length)
            this.cps[index].update(x, y)
    }

    update(){
    }
    

    handleMouseDown(evt) {
        console.log('[base_container]handle MouseDown')
        this._tempPoint = new Point(evt.stageX, evt.stageY);
    }
    handlePressMove(evt) {
        console.log('[base_container]handle PressMove')
        this.update();
    }
    handlePressUp(evt) {
        console.log('[base_container]handle PressUp')
        this._tempPoint = new Point(0, 0);
    }
    handleDbClick(evt) {
        console.log('[base_container]handle DbClick')
    }
}