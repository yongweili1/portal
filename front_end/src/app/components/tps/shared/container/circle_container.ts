import { BaseContainer } from '../container/base_container'
import { Circle } from '../overlay/circle'
import { ControlPoint } from '../overlay/controlpoint'
import { Text } from '../overlay/text'
import { Point } from '../tools/point';

export class CircleContainer extends BaseContainer {
    start: ControlPoint;
    circle: Circle;
    end: ControlPoint;
    text: Text;

    constructor(stage) {
        super(stage, 'circle');
        this.start = new ControlPoint(stage)
        this.circle = new Circle(stage)
        this.end = new ControlPoint(stage)
        this.text = new Text(stage, 'Length: * mm')
        this.addChild(this.circle, this.start, this.end, this.text)
    }

    update() {
        super.update()
        this.circle.update()
        this.start.update()
        this.end.update()
        this.updateText()
    }

    setStartPoint(point) {
        this.start.setCenter(point)
    }

    setEndPoint(point) {
        this.end.setCenter(point)
        this._setCircle()
    }

    updateText() {
        if (this.start.getCenter().x > this.end.getCenter().x) {
            this.text.setCp(this.start.getCenter())
        } else {
            this.text.setCp(this.end.getCenter())
        }
        let x_side = this.end.getCenter().x - this.start.getCenter().x
        let y_side = this.end.getCenter().y - this.start.getCenter().y
        let r = Math.sqrt(x_side * x_side + y_side * y_side) / 2
        let area = Math.PI * r * r;
        this.text.setText('Area: ' + area.toFixed(2) + ' pixel2')
        this.text.update()
    }

    _setCircle() {
        this.circle.setStartPoint(this.start.getCenter())
        this.circle.setEndPoint(this.end.getCenter())
    }

    handleMouseDown(evt) {
        console.log('[circle]handle MouseDown')
        super.handleMouseDown(evt)
        this.isMousedown = true;
        if (evt.target.type != 'circle' && evt.target.type != 'controlpoint' && evt.target.type != 'text')
            this.setStartPoint(new Point(evt.offsetX, evt.offsetY))
    }
    handleMouseMove(evt) {
        if (this.isMousedown) {
            console.log('[circle]handle MouseMove')
            this.isPaint = true;
            this.setEndPoint(new Point(evt.offsetX, evt.offsetY))
            this.update();
        }
    }
    handleMouseUp(evt) {
        console.log('[circle]handle MouseUp')
        this.isMousedown = false;
        this.isPaint = false;
    }

    handlePressMove(evt) {
        console.log('[circle]handle PressMove')
        let delta_x = evt.stageX - this._tempPoint.x;
        let delta_y = evt.stageY - this._tempPoint.y;
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;
        
        if (evt.target == this.circle || evt.target == this.text) {
            this.x += delta_x;
            this.y += delta_y;
        } else {
            evt.target.updateCenter(delta_x, delta_y)
        }

        this.update();
    }
}