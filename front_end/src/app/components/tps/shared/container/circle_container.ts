import { BaseContainer } from '../container/base_container'
import { Circle } from '../overlay/circle'
import { ControlPoint } from '../overlay/controlpoint'

export class CircleContainer extends BaseContainer {
    start: ControlPoint;
    circle: Circle;
    end: ControlPoint;

    constructor(stage) {
        super(stage, 'circle');
        this.start = new ControlPoint(stage)
        this.circle = new Circle(stage)
        this.end = new ControlPoint(stage)
        this.addChild(this.circle, this.start, this.end)
    }

    update() {
        super.update()
        this.circle.update()
        this.start.update()
        this.end.update()
    }

    setStartPoint(point) {
        this.start.setCenter(point)
    }

    setEndPoint(point) {
        this.end.setCenter(point)
        this._setCircle()
    }

    _setCircle() {
        this.circle.setStartPoint(this.start.getCenter())
        this.circle.setEndPoint(this.end.getCenter())
    }

    handlePressMove(evt) {
        let delta_x = evt.stageX - this._tempPoint.x;
        let delta_y = evt.stageY - this._tempPoint.y;
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;
        
        if (evt.target == this.circle) {
            this.x += delta_x;
            this.y += delta_y;
        } else {
            evt.target.updateCenter(delta_x, delta_y)
        }

        this.update();
    }
}