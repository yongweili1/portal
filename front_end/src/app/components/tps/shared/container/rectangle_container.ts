import { BaseContainer } from '../container/base_container'
import { Rectangle } from '../overlay/rectangle'
import { ControlPoint } from '../overlay/controlpoint'
import { Point } from '../tools/point'
import { Text } from '../overlay/text'

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
        super(stage, 'line');
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
    }

    update() {
        super.update()
        this.rectangle.update()
        this.top_left.update()
        this.top_center.update()
        this.top_right.update()
        this.left_center.update()
        this.right_center.update()
        this.bottom_left.update()
        this.bottom_center.update()
        this.bottom_right.update()
        this.updateText()
    }

    setStartPoint(point) {
        this.top_left.setCenter(point)
    }

    setEndPoint(point) {
        this.bottom_right.setCenter(point)
        this._setRectangle()
        this._updateCps()
    }

    updateText() {
        if (this.top_left.getCenter().x > this.top_right.getCenter().x) {
            this.text.setCp(this.top_left.getCenter())
        } else {
            this.text.setCp(this.top_right.getCenter())
        }
        let x_side = this.top_right.getCenter().x - this.top_left.getCenter().x
        let y_side = this.bottom_left.getCenter().y - this.top_left.getCenter().y
        let area = x_side * x_side + y_side * y_side
        this.text.setText('Area: ' + area.toFixed(2) + ' pixel2')
        this.text.update()
    }

    _setRectangle() {
        this.rectangle.setStartPoint(this.top_left.getCenter())
        this.rectangle.setEndPoint(this.bottom_right.getCenter())
    }

    _updateCps() {
        let start = this.top_left.getCenter()
        let end = this.bottom_right.getCenter()
        this.top_center.setCenter(new Point(start.x + (end.x - start.x) / 2, start.y))
        this.top_right.setCenter(new Point(end.x, start.y))
        this.left_center.setCenter(new Point(start.x, start.y + (end.y - start.y) / 2))
        this.right_center.setCenter(new Point(end.x, start.y + (end.y - start.y) / 2))
        this.bottom_left.setCenter(new Point(start.x, end.y))
        this.bottom_center.setCenter(new Point(start.x + (end.x - start.x) / 2, end.y))
    }

    handlePressMove(evt) {
        let delta_x = evt.stageX - this._tempPoint.x;
        let delta_y = evt.stageY - this._tempPoint.y;
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;
        if (evt.target == this.top_left) {
            this.top_left.updateCenter(delta_x, delta_y)
        } else if (evt.target == this.top_center) {
            this.top_left.updateCenter(null, delta_y)
        } else if (evt.target == this.top_right) {
            this.top_left.updateCenter(null, delta_y)
            this.bottom_right.updateCenter(delta_x, null)
        } else if (evt.target == this.left_center) {
            this.top_left.updateCenter(delta_x, null)
        } else if (evt.target == this.right_center) {
            this.bottom_right.updateCenter(delta_x, null)
        } else if (evt.target == this.bottom_left) {
            this.top_left.updateCenter(delta_x, null)
            this.bottom_right.updateCenter(null, delta_y)
        } else if (evt.target == this.bottom_center) {
            this.bottom_right.updateCenter(null, delta_y)
        } else if (evt.target == this.bottom_right) {
            this.bottom_right.updateCenter(delta_x, delta_y)
        } else {
            this.x += delta_x;
            this.y += delta_y;
        }

        this._setRectangle()
        this._updateCps()
        this.update();
    }
}