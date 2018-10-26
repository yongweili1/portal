import { BaseContainer } from '../container/base_container'
import { Line } from '../overlay/line'
import { ControlPoint } from '../overlay/controlpoint'
import { Point } from '../tools/point'
import { Output, EventEmitter } from '@angular/core';

export class CrosslineContainer extends BaseContainer {
    center: Point;
    horizontal: Line;
    vertical: Line;
    crossPoint: ControlPoint;
    @Output() SendCrossInfoEvent: EventEmitter<any> = new EventEmitter<any>();

    constructor(stage) {
        super(stage, 'crossline');
        this.horizontal = new Line(stage, 'green')
        this.vertical = new Line(stage, 'yellow')
        this.crossPoint = new ControlPoint(stage)
        this.setCenter(new Point(this.overlayStage.canvas.width / 2, this.overlayStage.canvas.height / 2))
        this.addChild(this.horizontal, this.vertical, this.crossPoint)
    }

    update() {
        super.update()
        // this.horizontal.update()
        // this.vertical.update()
        // this.crossPoint.update()
    }

    setCenter(point) {
        this.center = point;
        this.crossPoint.setCenter(point)
    }

    setHorizontal() {
        this.horizontal.setStartPoint(new Point(0, this.center.y))
        this.horizontal.setEndPoint(new Point(this.overlayStage.canvas.width, this.center.y))
    }

    setVertical() {
        this.vertical.setStartPoint(new Point(this.center.x, 0))
        this.vertical.setEndPoint(new Point(this.center.x, this.overlayStage.canvas.height))
    }

    handlePressMove(evt) {
        console.log('[crossline]handle PressMove')
        let delta_x = evt.stageX - this._tempPoint.x;
        let delta_y = evt.stageY - this._tempPoint.y;
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;

        if (evt.target == this.horizontal) {
            this.center.y += delta_y
        } else if (evt.target == this.vertical) {
            this.center.x += delta_x
        } else {
            this.center.x += delta_x
            this.center.y += delta_y
        }
        this.setCenter(this.center)
        this.setHorizontal()
        this.setVertical()
        this.update();
    }
}