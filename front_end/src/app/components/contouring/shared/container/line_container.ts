import { BaseContainer } from '../container/base_container';
import { Line } from '../overlay/line';
import { ControlPoint } from '../overlay/controlpoint';
import { Text } from '../overlay/text';
import { Point } from '../tools/point';
import { RoiModel } from '../model/roi.model';
import { ShapeTypeEnum, ContourTypeEnum } from '../../../../shared/models/enums';
import { Utils } from '../tools/utils';
import { EventAggregator } from '../../../../shared/common/event_aggregator';


export class LineContainer extends BaseContainer {
    start: ControlPoint;
    line: Line;
    end: ControlPoint;
    text: Text;
    cps: Array<Point>;
    utils = new Utils();

    constructor(stage) {
        super(stage, ShapeTypeEnum.line);
        this.contour_type = ContourTypeEnum.line;
        this.initCps(2);
        this.start = new ControlPoint(stage);
        this.line = new Line(stage);
        this.end = new ControlPoint(stage);
        this.text = new Text(stage, 'Length: * mm');
        this.addChild(this.line, this.start, this.end, this.text);
    }

    public setRoi(roi: RoiModel) {
        super.setRoi(roi);
        this.line.color = roi.color;
    }

    update() {
        super.update();
        this.start.update(this.cps[0]);
        this.line.update(this.cps[0], this.cps[1]);
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
        const length = Math.sqrt(x_side * x_side + y_side * y_side);
        this.text.setText('Length: ' + length.toFixed(2) + ' pixel');
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
        console.log('[line]handle MouseDown');
        super.handleMouseDown(evt);
        this.isMousedown = true;
        if (evt.target.type !== ShapeTypeEnum.line
            && evt.target.type !== ShapeTypeEnum.controlpoint
            && evt.target.type !== ShapeTypeEnum.text) {
            this.updateCp(0, evt.offsetX, evt.offsetY);
        }
    }
    handleMouseMove(evt) {
        if (this.isMousedown) {
            console.log('[line]handle MouseMove');
            this.isPaint = true;

            const mousePt = new Point(evt.offsetX, evt.offsetY);
            if (!this.utils.isInPolygon(mousePt, this.boundaryPts)) {
                // Plan A: 求最近点
                // const intersection = this.utils.getNearestPt(this.activeAreaBoundaryPts, mousePt);
                // Plan B: 求鼠标指针和最后一个cp连线与边框的交点，会导致图元紧贴边界的时候运动十分缓慢
                const intersection = this.utils.getShapeIntersectionWithLine(this.activeAreaBoundaryPts, [this.cps[0], mousePt]);
                this.updateCp(1, intersection.x, intersection.y);
            } else {
                this.updateCp(1, evt.offsetX, evt.offsetY);
            }

            this.update();
        }
    }
    handleMouseUp(evt) {
        console.log('[line]handle MouseUp');
        this.isMousedown = false;
        this.isPaint = false;
        EventAggregator.Instance().contourReadyEvent.publish([[this.cps], this.contour_type, this.contour_uid]);
    }

    handlePressUp(evt) {
        if (this.roiConfig.id === this.selectedRoi_id) {
            super.handlePressUp(evt);
            EventAggregator.Instance().updateSigleContourEvent.publish([this.cps, this.contour_uid]);
        }
    }

    handlePressMove(evt) {
        console.log('[line]handle PressMove');
        if (this.roiConfig.id === this.selectedRoi_id) {
            const delta_x = evt.stageX - this._tempPoint.x;
            const delta_y = evt.stageY - this._tempPoint.y;
            this._tempPoint.x = evt.stageX;
            this._tempPoint.y = evt.stageY;

            const tempCp0: Point = this.cps[0].copy();
            const tempCp1: Point = this.cps[1].copy();

            if (evt.target === this.line || evt.target === this.text) {
                tempCp0.offset(delta_x, delta_y);
                tempCp1.offset(delta_x, delta_y);
            } else if (evt.target === this.start) {
                tempCp0.offset(delta_x, delta_y);
            } else if (evt.target === this.end) {
                tempCp1.offset(delta_x, delta_y);
            }
            if (this.utils.isInPolygon(tempCp0, this.boundaryPts) && this.utils.isInPolygon(tempCp1, this.boundaryPts)) {
                this.cps[0] = tempCp0;
                this.cps[1] = tempCp1;
                this.update();
            }
        }
    }
}
