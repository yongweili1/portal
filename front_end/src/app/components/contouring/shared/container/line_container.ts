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
    cps: Array<Point>;
    
    constructor(stage) {
        super(stage, 'line');
        this.initCps(2)
        this.start = new ControlPoint(stage)
        this.line = new Line(stage)
        this.end = new ControlPoint(stage)
        this.text = new Text(stage, 'Length: * mm')
        this.addChild(this.line, this.start, this.end, this.text)
    }

    update() {
        super.update()
        this.start.update(this.cps[0])
        this.line.update(this.cps[0], this.cps[1])
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
        let length = Math.sqrt(x_side * x_side + y_side * y_side);
        this.text.setText('Length: ' + length.toFixed(2) + ' pixel')
        this.text.update()
    }

    handleMouseDown(evt) {
        console.log('[line]handle MouseDown')
        super.handleMouseDown(evt)
        this.isMousedown = true;
        if (evt.target.type != 'line' && evt.target.type != 'controlpoint' && evt.target.type != 'text') {
            this.updateCp(0, evt.offsetX, evt.offsetY)
        }
    }
    handleMouseMove(evt) {
        if (this.isMousedown) {
            console.log('[line]handle MouseMove')
            this.isPaint = true;
            this.updateCp(1, evt.offsetX, evt.offsetY)
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