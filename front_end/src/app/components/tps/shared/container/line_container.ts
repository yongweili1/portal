import { BaseContainer } from '../container/base_container'
import { Line } from '../overlay/line'
import { ControlPoint } from '../overlay/controlpoint'

export class LineContainer extends BaseContainer {
    start: ControlPoint;
    line: Line;
    end: ControlPoint;

    constructor(stage) {
        super(stage, 'line');
        this.start = new ControlPoint(stage)
        this.line = new Line(stage)
        this.end = new ControlPoint(stage)
        this.addChild(this.line, this.start, this.end)
    }

    update() {
        super.update()
        this.start.update()
        this.line.update()
        this.end.update()
    }

    setStartPoint(point) {
        this.start.setCenter(point)
    }

    setEndPoint(point) {
        this.end.setCenter(point)
        this._setLine()
    }

    _setLine() {
        this.line.setStartPoint(this.start.getCenter())
        this.line.setEndPoint(this.end.getCenter())
    }

    handlePressMove(evt) {
        let delta_x = evt.stageX - this._tempPoint.x;
        let delta_y = evt.stageY - this._tempPoint.y;
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;
        
        if (evt.target == this.line) {
            this.x += delta_x;
            this.y += delta_y;
        } else {
            evt.target.updateCenter(delta_x, delta_y)
        }

        this.update();
    }
}