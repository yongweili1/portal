import { BaseContainer } from '../container/base_container';
import { Circle } from '../overlay/circle';
import { ControlPoint } from '../overlay/controlpoint';
import { Text } from '../overlay/text';
import { RoiModel } from '../model/roi.model';
import { ShapeTypeEnum } from '../../../../shared/models/enums';
import { Utils } from '../tools/utils';
import { Point } from '../tools/point';

export class CircleContainer extends BaseContainer {
    start: ControlPoint;
    circle: Circle;
    end: ControlPoint;
    text: Text;
    utils = new Utils();

    constructor(stage) {
        super(stage, ShapeTypeEnum.circle);
        this.start = new ControlPoint(stage);
        this.circle = new Circle(stage);
        this.end = new ControlPoint(stage);
        this.text = new Text(stage, 'Length: * mm');
        this.addChild(this.circle, this.start, this.end, this.text);
        this.initCps(2);
    }

    public setRoi(roi: RoiModel) {
        super.setRoi(roi);
        this.circle.color = roi.color;
    }

    update() {
        super.update();
        this.start.update(this.cps[0]);
        this.circle.update(this.cps[0], this.cps[1]);
        this.end.update(this.cps[1]);
        this.updateText();
    }

    updateText() {
        if (this.cps[0].x > this.cps[1].x) {
            this.text.setCp(this.cps[0]);
        } else {
            this.text.setCp(this.cps[1]);
        }
        const x_side = this.cps[1].x - this.cps[0].x;
        const y_side = this.cps[1].y - this.cps[0].y;
        const r = Math.sqrt(x_side * x_side + y_side * y_side) / 2;
        const area = Math.PI * r * r;
        this.text.setText('Area: ' + area.toFixed(2) + ' pixel');
        this.text.update();
    }

    public setBoundaryPts(pts) {
        super.setBoundaryPts(pts);
        this.activeAreaBoundaryPts = this.utils.scaleRectangleBoundary(pts, -1);
    }

    handleMouseDown(evt) {
        console.log('[circle]handle MouseDown');
        super.handleMouseDown(evt);
        this.isMousedown = true;
        if (evt.target.type !== 'circle'
            && evt.target.type !== 'controlpoint'
            && evt.target.type !== 'text') {
            this.updateCp(0, evt.offsetX, evt.offsetY);
        }
    }
    handleMouseMove(evt) {
        if (this.isMousedown) {
            console.log('[circle]handle MouseMove');
            const evtPoint = new Point(evt.offsetX, evt.offsetY);
            const centerPoint = this.utils.getCenterPoint(this.cps[0], evtPoint);
            //diameter圆的直径
            const diameter = this.utils.getLengthOfTwoPoint(this.cps[0], evtPoint);
            const lengthToRec =this.utils.getLengthOfTwoPoint(this.utils.getNearestPt(this.boundaryPts,centerPoint),centerPoint);
            console.log("lengthToRec is " + lengthToRec);
            console.log("diameter is " + diameter);
            this.isPaint = true;
            if (lengthToRec >= diameter / 2) {
                this.updateCp(1, evt.offsetX, evt.offsetY);
                this.update();
            }
        }
    }
    handleMouseUp(evt) {
        console.log('[circle]handle MouseUp');
        this.isMousedown = false;
        this.isPaint = false;
    }

    handlePressMove(evt) {
        console.log('[circle]handle PressMove');
        const delta_x = evt.stageX - this._tempPoint.x;
        const delta_y = evt.stageY - this._tempPoint.y;
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;

        if (evt.target === this.circle || evt.target === this.text) {
            this.cps[0].offset(delta_x, delta_y);
            this.cps[1].offset(delta_x, delta_y);
        } else if (evt.target === this.start) {
            this.cps[0].offset(delta_x, delta_y);
        } else if (evt.target === this.end) {
            this.cps[1].offset(delta_x, delta_y);
        }

        this.update();
    }
}
