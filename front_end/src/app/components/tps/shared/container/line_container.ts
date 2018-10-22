import { BaseContainer } from '../container/base_container'
import { Line } from '../overlay/line'
import { ControlPoint } from '../overlay/controlpoint'
import { Text } from '../overlay/text'
import { Point } from '../tools/point';

export class LineContainer extends BaseContainer {
    start: ControlPoint;
    line: Line;
    end: ControlPoint;
    text: Text;

    constructor(stage) {
        super(stage, 'line');
        this.start = new ControlPoint(stage)
        this.line = new Line(stage)
        this.end = new ControlPoint(stage)
        this.text = new Text(stage, 'Length: * mm')
        this.addChild(this.line, this.start, this.end, this.text)
    }

    update() {
        super.update()
        this.start.update()
        this.line.update()
        this.end.update()
        this.updateText()
    }

    setStartPoint(point) {
        this.start.setCenter(point)
    }

    setEndPoint(point) {
        this.end.setCenter(point)
        this._setLine()
    }

    updateText() {
        if (this.start.getCenter().x > this.end.getCenter().x) {
            this.text.setCp(this.start.getCenter())
        } else {
            this.text.setCp(this.end.getCenter())
        }
        let x_side = this.end.getCenter().x - this.start.getCenter().x
        let y_side = this.end.getCenter().y - this.start.getCenter().y
        let length = Math.sqrt(x_side * x_side + y_side * y_side);
        this.text.setText('Length: ' + length.toFixed(2) + ' pixel')
        this.text.update()
    }

    _setLine() {
        this.line.setStartPoint(this.start.getCenter())
        this.line.setEndPoint(this.end.getCenter())
    }

    handleMouseDown(evt) {
        console.log('[line]handle MouseDown')
        super.handleMouseDown(evt)
        this.isMousedown = true;
        if (evt.target.type != 'line' && evt.target.type != 'controlpoint' && evt.target.type != 'text')
            this.setStartPoint(new Point(evt.offsetX, evt.offsetY))
    }
    handleMouseMove(evt) {
        if (this.isMousedown) {
            console.log('[line]handle MouseMove')
            this.isPaint = true;
            this.setEndPoint(new Point(evt.offsetX, evt.offsetY))
            this.update();
        }
    }
    handleMouseUp(evt) {
        console.log('[line]handle MouseUp')
        this.isMousedown = false;
        this.isPaint = false;
    }

    handlePressMove(evt) {
        console.log('[line]handle PressMove')
        let delta_x = evt.stageX - this._tempPoint.x;
        let delta_y = evt.stageY - this._tempPoint.y;
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;
        
        if (evt.target == this.line || evt.target == this.text) {
            this.x += delta_x;
            this.y += delta_y;
        } else {
            evt.target.updateCenter(delta_x, delta_y)
        }

        this.update();
    }
}