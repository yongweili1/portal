import { BaseContainer } from '../container/base_container'
import { Rectangle } from '../overlay/rectangle'
import { ControlPoint } from '../overlay/controlpoint'
import { Point } from '../tools/point'
import { Text } from '../overlay/text'
import { ROIConfig } from '../model/ROIConfig.model';

export class RectangleContainer extends BaseContainer {
    rectangle: Rectangle;
    top_left: ControlPoint;
    top_center: ControlPoint;
    top_right: ControlPoint;
    left_center: ControlPoint;
    right_center: ControlPoint;
    bottom_left: ControlPoint;
    bottom_center: ControlPoint;
    bottom_right: ControlPoint;
    text: Text;

    constructor(stage) {
        super(stage, 'rectangle');
        this.top_left = new ControlPoint(stage)
        this.top_center = new ControlPoint(stage)
        this.top_right = new ControlPoint(stage)
        this.left_center = new ControlPoint(stage)
        this.right_center = new ControlPoint(stage)
        this.bottom_left = new ControlPoint(stage)
        this.bottom_center = new ControlPoint(stage)
        this.bottom_right = new ControlPoint(stage)
        this.rectangle = new Rectangle(stage)
        this.text = new Text(stage, 'Length: * mm')
        this.addChild(this.rectangle, this.top_left, this.top_center, 
                      this.top_right, this.left_center, this.right_center, 
                      this.bottom_left, this.bottom_center, this.bottom_right, this.text)
        this.initCps(8)
    }

    public setRoi(roi: ROIConfig) {
        super.setRoi(roi)
        this.rectangle.color = roi.ROIColor;
    }

    update() {
        super.update()
        this._updateFollowCps()
        this.rectangle.update(this.cps[0], this.cps[7])
        this.top_left.update(this.cps[0])
        this.top_center.update(this.cps[1])
        this.top_right.update(this.cps[2])
        this.left_center.update(this.cps[3])
        this.right_center.update(this.cps[4])
        this.bottom_left.update(this.cps[5])
        this.bottom_center.update(this.cps[6])
        this.bottom_right.update(this.cps[7])
        this.updateText()
    }

    updateText() {
        if (this.cps[0].x > this.cps[2].x) {
            this.text.setCp(this.cps[0])
        } else {
            this.text.setCp(this.cps[2])
        }
        let x_side = this.cps[2].x - this.cps[0].x
        let y_side = this.cps[5].y - this.cps[0].y
        let area = x_side * x_side + y_side * y_side
        this.text.setText('Area: ' + area.toFixed(2) + ' pixel')
        this.text.update()
    }

    _updateFollowCps() {
        let start = this.cps[0]
        let end = this.cps[7]
        this.updateCp(1, start.x + (end.x - start.x) / 2, start.y)
        this.updateCp(2, end.x, start.y)
        this.updateCp(3, start.x, start.y + (end.y - start.y) / 2)
        this.updateCp(4, end.x, start.y + (end.y - start.y) / 2)
        this.updateCp(5, start.x, end.y)
        this.updateCp(6, start.x + (end.x - start.x) / 2, end.y)
    }

    handleMouseDown(evt) {
        console.log('[rectangle]handle MouseDown')
        super.handleMouseDown(evt)
        this.isMousedown = true;
        if (evt.target.type != 'rectangle' && evt.target.type != 'controlpoint' && evt.target.type != 'text')
            this.updateCp(0, evt.offsetX, evt.offsetY)
    }
    handleMouseMove(evt) {
        if (this.isMousedown) {
            console.log('[rectangle]handle MouseMove')
            this.isPaint = true;
            this.updateCp(7, evt.offsetX, evt.offsetY)
            this.update();
        }
    }
    handleMouseUp(evt) {
        console.log('[rectangle]handle MouseUp')
        this.isMousedown = false;
        this.isPaint = false;
    }

    handlePressMove(evt) {
        console.log('[rectangle]handle PressMove')
        let delta_x = evt.stageX - this._tempPoint.x;
        let delta_y = evt.stageY - this._tempPoint.y;
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;
        
        if (evt.target == this.top_left) {
            this.cps[0].offset(delta_x, delta_y)
        } else if (evt.target == this.top_center) {
            this.cps[0].offset(null, delta_y)
        } else if (evt.target == this.top_right) {
            this.cps[0].offset(null, delta_y)
            this.cps[7].offset(delta_x, null)
        } else if (evt.target == this.left_center) {
            this.cps[0].offset(delta_x, null)
        } else if (evt.target == this.right_center) {
            this.cps[7].offset(delta_x, null)
        } else if (evt.target == this.bottom_left) {
            this.cps[0].offset(delta_x, null)
            this.cps[7].offset(null, delta_y)
        } else if (evt.target == this.bottom_center) {
            this.cps[7].offset(null, delta_y)
        } else if (evt.target == this.bottom_right) {
            this.cps[7].offset(delta_x, delta_y)
        } else {
            this.cps[0].offset(delta_x, delta_y)
            this.cps[7].offset(delta_x, delta_y)
        }

        this.update();
    }
}