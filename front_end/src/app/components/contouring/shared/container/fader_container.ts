import { BaseContainer } from './base_container';
import { Fader } from '../overlay/fader';
import { Point } from '../tools/point';
import { RoiModel } from '../model/roi.model';
import { Line } from '../overlay/line';
import { ShapeTypeEnum, ContourTypeEnum } from '../../../../shared/models/enums';
import { Utils } from '../tools/utils';
import { EventAggregator } from '../../../../shared/common/event_aggregator';

export class FaderContainer extends BaseContainer {
    fader: Fader;
    prePos: Point;
    horizontal: Line;
    vertical: Line;

    // none, none, 0
    // positive, +, 1
    // negative, -, -1
    state = 0;

    utils = new Utils();

    constructor(stage) {
        super(stage, ShapeTypeEnum.fader);
        this.contour_type = ContourTypeEnum.fader;
        this.fader = new Fader(stage);
        this.horizontal = new Line(stage);
        this.vertical = new Line(stage);
        this.addChild(this.horizontal, this.vertical, this.fader);
    }

    public setRoi(roi: RoiModel) {
        if (roi === undefined) {
            return;
        }
        super.setRoi(roi);
        this.fader.color = roi.color;
        this.horizontal.color = roi.color;
        this.vertical.color = roi.color;
    }

    update() {
        this.showState();
        this.fader.update(this.fader.center, this.isFill);
    }

    showState(state = 0) {
        if (state !== 0) {
            this.state = state;
        }
        const p = this.getCenter().copy();
        this.horizontal.update(p.copy().offset(-this.getRadius() / 2, 0), p.copy().offset(this.getRadius() / 2, 0));
        if (this.state === 1) { // positive, +
            this.vertical.update(p.copy().offset(0, -this.getRadius() / 2), p.copy().offset(0, this.getRadius() / 2));
        } else { // negative, - or none
            this.vertical.update(this.getCenter(), this.getCenter());
        }
    }

    getCenter() {
        return this.fader.center;
    }

    getRadius() {
        return this.fader.radius;
    }

    setRadius(value) {
        this.fader.radius = value;
    }

    updateRadius(delta: number) {
        this.fader.radius += delta;
        this.update();
    }

    getCps() {
        return this.fader.cps;
    }

    public setBoundaryPts(pts) {
        if (pts === undefined) {
            return;
        }
        super.setBoundaryPts(pts);
        this.activeAreaBoundaryPts = this.utils.scaleRectangleBoundary(pts, -this.fader.radius);
    }

    handleMouseDown(evt) {
        super.handleMouseDown(evt);

        if (evt.offsetX === undefined || evt.offsetY === undefined) {
            this.prePos = new Point(evt.stageX, evt.stageY);
        } else {
            this.prePos = new Point(evt.offsetX, evt.offsetY);
        }

        this.fader.setCenter(this.prePos);
        this.update();
    }

    handleMouseMove(e) {
        if (this.activeAreaBoundaryPts === undefined) {
            return;
        }
        const mousePt = new Point(e.offsetX, e.offsetY);
        if (!this.utils.isInPolygon(mousePt, this.activeAreaBoundaryPts)) {
            // Plan A: 求最近点
            const intersection = this.utils.getNearestPt(this.activeAreaBoundaryPts, mousePt);
            // Plan B: 求鼠标指针和最后一个cp连线与边框的交点，会导致图元紧贴边界的时候运动十分缓慢
            // const lineCps = [this.fader.center, mousePt];
            // let intersection = this.utils.getShapeIntersectionWithLine(this.activeAreaBoundaryPts, lineCps);
            // const center_x = (this.boundaryPts[0].x + this.boundaryPts[2].x) / 2;
            // const center_y = (this.boundaryPts[0].y + this.boundaryPts[2].y) / 2;
            // intersection = this.utils.closeTo(intersection, new Point(center_x, center_y));
            if (intersection != null) {
                this.fader.setCenter(intersection);
            }
        } else {
            this.fader.setCenter(mousePt);
        }
        this.update();
    }

    handleMouseUp(e) {
        super.handleMouseUp(e);
        this.isPaint = false;
        EventAggregator.Instance().contourReadyEvent.publish(this.contour_type);
    }

    handlePressMove(e) {
    }

    handleMouseLeave(e) {
    }
}
