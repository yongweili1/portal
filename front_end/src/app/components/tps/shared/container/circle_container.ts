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
        this.initCps(2)
    }

    update() {
        super.update()
        this.start.update(this.cps[0])
        this.circle.update(this.cps[0], this.cps[1])
        this.end.update(this.cps[1])
        this.updateText()
    }

    updateText() {
        if (this.cps[0].x > this.cps[1].x) {
            this.text.setCp(this.cps[0])
        } else {
            this.text.setCp(this.cps[1])
        }
        let x_side = this.cps[1].x - this.cps[0].x
        let y_side = this.cps[1].y - this.cps[0].y
        let r = Math.sqrt(x_side * x_side + y_side * y_side) / 2
        let area = Math.PI * r * r;
        this.text.setText('Area: ' + area.toFixed(2) + ' pixel')
        this.text.update()
    }

    handleMouseDown(evt) {
        console.log('[circle]handle MouseDown')
        super.handleMouseDown(evt)
        this.isMousedown = true;
        if (evt.target.type != 'circle' && evt.target.type != 'controlpoint' && evt.target.type != 'text')
            this.updateCp(0, evt.offsetX, evt.offsetY)
    }
    handleMouseMove(evt) {
        if (this.isMousedown) {
            console.log('[circle]handle MouseMove')
            this.isPaint = true;
            this.updateCp(1, evt.offsetX, evt.offsetY)
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
            this.cps[0].offset(delta_x, delta_y)
            this.cps[1].offset(delta_x, delta_y)
        } else if (evt.target == this.start) {
            this.cps[0].offset(delta_x, delta_y)
        } else if (evt.target == this.end) {
            this.cps[1].offset(delta_x, delta_y)
        }

        this.update();
    }
}