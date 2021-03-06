import { BaseContainer } from '../container/base_container';
import { Freepen } from '../overlay/freepen';
import { Point, Point3d } from '../tools/point';
import { Text } from '../overlay/text';
import { RoiModel } from '../model/roi.model';
import { ShapeTypeEnum, ContourTypeEnum } from '../../../../shared/models/enums';
import { Utils } from '../tools/utils';
import { EventAggregator } from '../../../../shared/common/event_aggregator';

export class FreepenContainer extends BaseContainer {
    shape: Freepen;
    text: Text;
    editable: false;
    nearestStartCp: any;
    nearestEndCp: any;

    extendShape: Freepen;
    extendCps: Array<Point>;
    utils = new Utils();

    constructor(stage) {
        super(stage, ShapeTypeEnum.freepen);
        this.contour_type = ContourTypeEnum.freepen;
        this.shape = new Freepen(stage);
        this.extendShape = new Freepen(stage);
        this.addChild(this.extendShape);
        this.initCps();
        this.extendCps = new Array();
        this.text = new Text(stage, '* cps');
        this.addChild(this.shape, this.text);
    }

    public setRoi(roi: RoiModel) {
        super.setRoi(roi);
        this.shape.color = roi.color;
        this.extendShape.color = roi.color;
    }

    update() {
        super.update();
        this.shape.update(this.cps, this.isFill);
        this.extendShape.update(this.extendCps, this.isFill);
        // this.cps.forEach(cp => {
        //     cp.update()
        // });
        this.updateText();
    }

    setCps(cps) {
        this.cps = cps;
    }

    clearPaint() {
        this.extendCps = new Array();
        this.extendShape.update(this.extendCps, this.isFill);
    }

    getNearestCp(p: Point) {
        let index = 0;
        let nearestCp = this.cps[0];
        let distance = this.getDistance(p, nearestCp);
        for (let i = 0; i < this.cps.length; i++) {
            const cp = this.cps[i];
            const dis = Math.sqrt((cp.x - p.x) ** 2 + (cp.y - p.y) ** 2);
            if (dis < distance) {
                index = i;
                distance = dis;
                nearestCp = cp;
            }
        }

        return [index, nearestCp];
    }

    getLength(start, end, points) {
        let length = 0;
        for (let index = start + 1; index < end; index++) {
            length += this.getDistance(points[index - 1], points[index]);
        }
        return length;
    }

    getDistance(p1: Point, p2: Point) {
        return Math.sqrt((p1.x - p2.x) ** 2 + (p1.y - p2.y) ** 2);
    }

    updateText() {
        this.text.setCp(this.getRightCp());
        this.text.setText('ControlPoint: ' + this.cps.length);
        this.text.update();
    }

    getRightCp() {
        let p = this.cps[0];
        this.cps.forEach(cp => {
            if (cp.x > p.x) {
                p = cp;
            }
        });
        return p;
    }
    compareCp(cp1, cp2) {
        if (cp1.x === cp2.x && cp1.y === cp2.y) {
            return true;
        } else {
            return false;
        }
    }
    modifyShape() {
        let range = [this.nearestStartCp[0], this.nearestEndCp[0]];
        if (this.nearestStartCp[0] > this.nearestEndCp[0]) {
            range = [this.nearestEndCp[0], this.nearestStartCp[0]];
        }
        const totalDistance = this.getLength(0, this.cps.length, this.cps);
        const distance = this.getLength(range[0], range[1], this.cps);
        const _cps = new Array();
        if (totalDistance - distance > distance) {
            console.log('A');
            for (let index = 0; index < range[0]; index++) {
                _cps.push(this.cps[index]);
            }
            if (!this.compareCp(this.extendCps[0], this.cps[range[0]])) {
                this.extendCps.reverse();
            }
            this.extendCps.splice(0, 1);
            this.extendCps.pop();
            _cps.push.apply(_cps, this.extendCps);
            for (let index = range[1] + 1; index < this.cps.length; index++) {
                _cps.push(this.cps[index]);
            }
        } else {
            console.log('B');
            for (let index = range[0] + 1; index < range[1]; index++) {
                const cp = this.cps[index];
                _cps.push(cp);
            }
            if (!this.compareCp(this.extendCps[0], this.cps[range[1]])) {
                this.extendCps.reverse();
            }
            this.extendCps.splice(0, 1);
            this.extendCps.pop();
            _cps.push.apply(_cps, this.extendCps);
        }

        _cps.push(_cps[0].copy());
        this.cps = _cps;
        this.clearPaint();
    }

    setBoundaryPts(pts) {
        super.setBoundaryPts(pts);
        this.activeAreaBoundaryPts = this.utils.scaleRectangleBoundary(pts, -1);
    }

    validate() {
        super.validate();
        if (this.cps.length < 2) {
            return false;
        }
        return true;
    }

    handleMouseDown(evt) {
        console.log('[freepen]handle MouseDown');
        super.handleMouseDown(evt);
        this.isMousedown = true;

        if (this.editable) {
            const point = new Point(evt.stageX, evt.stageY);
            this.nearestStartCp = this.getNearestCp(point);
            this.extendCps.push(this.nearestStartCp[1]);
            this.extendCps.push(point);
        } else {
            if (evt.target.type !== ShapeTypeEnum.freepen
                && evt.target.type !== ShapeTypeEnum.controlpoint
                && evt.target.type !== ShapeTypeEnum.text) {
                this.cps.push(new Point(evt.offsetX, evt.offsetY));
            }
        }
    }
    handleMouseMove(evt) {
        if (this.isMousedown) {
            console.log('[freepen]handle MouseMove');
            this.isPaint = true;

            if (this.editable) {
                const point = new Point(evt.stageX, evt.stageY);
                this.extendCps.push(point);
                this.update();
            } else {
                console.log('push a new point');
                const mousePt = new Point(evt.offsetX, evt.offsetY);
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
                        this.cps.push(intersection);
                    }
                } else {
                    this.cps.push(mousePt);
                }
                this.update();
            }
        }
    }
    handleMouseUp(evt) {
        console.log('[freepen]handle MouseUp');
        if (this.isPaint) {
            if (this.editable) {
                const point = new Point(evt.stageX, evt.stageY);
                this.extendCps.push(point);

                this.nearestEndCp = this.getNearestCp(point);
                this.extendCps.push(this.nearestEndCp);

                this.modifyShape();
                this.update();
            } else {
                console.log('[freepen]close the shape');
                this.cps.push(this.cps[0].copy());
                this.update();
            }
        }
        this.isMousedown = false;
        this.isPaint = false;
        EventAggregator.Instance().contourReadyEvent.publish(this.contour_type);
    }
    handlePressUp(evt) {
        if (this.roiConfig.id === this.selectedRoi_id) {
            super.handlePressUp(evt);
            this.handleMouseUp(evt);
        }
    }

    handlePressMove(evt) {
        console.log('[freepen]handle PressMove');
        if (this.roiConfig.id === this.selectedRoi_id) {
            const delta_x = evt.stageX - this._tempPoint.x;
            const delta_y = evt.stageY - this._tempPoint.y;
            this._tempPoint.x = evt.stageX;
            this._tempPoint.y = evt.stageY;

            if (this.editable) {
                this.handleMouseMove(evt);
                return;
            }
            const tempCps: Array<Point> = new Array<Point>();
            console.log('boudaryPts : ' + this.boundaryPts);
            if (evt.target === this.shape || evt.target === this.text) {
                const res = this.cps.every(cp => {
                    const _temp = cp.copy();
                    _temp.offset(delta_x, delta_y);
                    if (!this.utils.isInPolygon(_temp, this.boundaryPts)) {
                        console.log('out of polygon');
                        return false;
                    }
                    console.log('is in polygon');
                    tempCps.push(_temp);
                    return true;
                });
                if (res) {
                    this.cps = tempCps;
                    this.update();
                }
            }
        }
    }
}
