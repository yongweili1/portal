import { BaseContainer } from '../container/base_container';
import { Circle } from '../overlay/circle';
import { ControlPoint } from '../overlay/controlpoint';
import { Text } from '../overlay/text';
import { RoiModel } from '../model/roi.model';
import { ShapeTypeEnum, ContourTypeEnum } from '../../../../shared/models/enums';
import { Utils } from '../tools/utils';
import { Point } from '../tools/point';
import { EventAggregator } from '../../../../shared/common/event_aggregator';

export class CircleContainer extends BaseContainer {
    start: ControlPoint;
    circle: Circle;
    end: ControlPoint;
    text: Text;
    utils = new Utils();

    constructor(stage) {
        super(stage, ShapeTypeEnum.circle);
        this.contour_type = ContourTypeEnum.circle;
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
        this.circle.update(this.cps[0], this.cps[1], this.isFill);
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

    validate() {
        super.validate();
        if (this.cps.length !== 2) {
            return false;
        }
        if (this.cps[0].equals(this.cps[1])) {
            return false;
        }
        return true;
    }

    handleMouseDown(evt) {
        console.log('[circle]handle MouseDown');
        super.handleMouseDown(evt);
        this.isMousedown = true;
        if (evt.target.type !== ShapeTypeEnum.circle
            && evt.target.type !== ShapeTypeEnum.controlpoint
            && evt.target.type !== ShapeTypeEnum.text) {
            this.updateCp(0, evt.offsetX, evt.offsetY);
        }
    }
    handleMouseMove(evt) {
        if (this.isMousedown) {
            console.log('[circle]handle MouseMove');
            const evtPoint = new Point(evt.offsetX, evt.offsetY);
            const centerPoint = this.utils.getCenterPoint(this.cps[0], evtPoint);
            // diameter圆的直径
            const diameter = this.utils.getLengthOfTwoPoint(this.cps[0], evtPoint);
            const nearestPt = this.utils.getNearestPt(this.boundaryPts, centerPoint);
            const lengthToRec = this.utils.getLengthOfTwoPoint(nearestPt, centerPoint);
            console.log('lengthToRec is', lengthToRec);
            console.log('diameter is', diameter);
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
        EventAggregator.Instance().contourReadyEvent.publish([[this.cps], this.contour_type, this.contour_uid]);
    }

    handlePressUp(evt) {
        super.handlePressMove(evt);
        EventAggregator.Instance().updateSigleContourEvent.publish([[this.cps], this.contour_uid]);
    }

    handlePressMove(evt) {
        console.log('[circle]handle PressMove');
        const evtPoint = new Point(evt.stageX, evt.stageY);
        if (this.utils.isInPolygon(evtPoint, this.boundaryPts)) {
            const delta_x = evt.stageX - this._tempPoint.x;
            const delta_y = evt.stageY - this._tempPoint.y;
            this.updateCircleByDelta(evt, delta_x, delta_y);
        }
    }

    updateCircleByDelta(evt, delta_x, delta_y) {
        const tempCps0 = this.cps[0].copy();
        const tempCps1 = this.cps[1].copy();
        switch (evt.target) {
            case this.start:
                tempCps0.offset(delta_x, delta_y);
                break;
            case this.end:
                tempCps1.offset(delta_x, delta_y);
                break;
            default:
                tempCps0.offset(delta_x, delta_y);
                tempCps1.offset(delta_x, delta_y);
                break;
        }
        const centerPoint = this.utils.getCenterPoint(tempCps0, tempCps1);
        const diameter = this.utils.getLengthOfTwoPoint(tempCps0, tempCps1);
        const nearestPt = this.utils.getNearestPt(this.boundaryPts, centerPoint);
        const lenFromCenterPtToRec = this.utils.getLengthOfTwoPoint(nearestPt, centerPoint);
        if (lenFromCenterPtToRec >= diameter / 2) {
            this.cps[0] = tempCps0;
            this.cps[1] = tempCps1;
            this.update();
            this._tempPoint.x = evt.stageX;
            this._tempPoint.y = evt.stageY;
        }
    }
}
