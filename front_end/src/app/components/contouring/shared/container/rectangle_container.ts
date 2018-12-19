import { BaseContainer } from '../container/base_container';
import { Rectangle } from '../overlay/rectangle';
import { ControlPoint } from '../overlay/controlpoint';
import { Text } from '../overlay/text';
import { RoiModel } from '../model/roi.model';
import { ShapeTypeEnum, ContourTypeEnum } from '../../../../shared/models/enums';
import { Utils } from '../tools/utils';
import { Point } from '../tools/point';
import { EventAggregator } from '../../../../shared/common/event_aggregator';


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
    utils = new Utils();

    constructor(stage) {
        super(stage, ShapeTypeEnum.rectangle);
        this.contour_type = ContourTypeEnum.rectangle;
        this.top_left = new ControlPoint(stage);
        this.top_center = new ControlPoint(stage);
        this.top_right = new ControlPoint(stage);
        this.left_center = new ControlPoint(stage);
        this.right_center = new ControlPoint(stage);
        this.bottom_left = new ControlPoint(stage);
        this.bottom_center = new ControlPoint(stage);
        this.bottom_right = new ControlPoint(stage);
        this.rectangle = new Rectangle(stage);
        this.text = new Text(stage, 'Length: * mm');
        this.addChild(this.rectangle, this.top_left, this.top_center,
            this.top_right, this.left_center, this.right_center,
            this.bottom_left, this.bottom_center, this.bottom_right, this.text);
        this.initCps(8);
    }

    public setRoi(roi: RoiModel) {
        super.setRoi(roi);
        this.rectangle.color = roi.color;
    }

    update() {
        super.update();
        this._updateFollowCps();
        this.rectangle.update(this.cps[0], this.cps[7], this.isFill);
        this.top_left.update(this.cps[0]);
        this.top_center.update(this.cps[1]);
        this.top_right.update(this.cps[2]);
        this.left_center.update(this.cps[3]);
        this.right_center.update(this.cps[4]);
        this.bottom_left.update(this.cps[5]);
        this.bottom_center.update(this.cps[6]);
        this.bottom_right.update(this.cps[7]);
        this.updateText();
    }

    updateText() {
        if (this.cps[0].x > this.cps[2].x) {
            this.text.setCp(this.cps[0]);
        } else {
            this.text.setCp(this.cps[2]);
        }
        const x_side = this.cps[2].x - this.cps[0].x;
        const y_side = this.cps[5].y - this.cps[0].y;
        const area = x_side * x_side + y_side * y_side;
        this.text.setText('Area: ' + area.toFixed(2) + ' pixel');
        this.text.update();
    }

    _updateFollowCps() {
        const start = this.cps[0];
        const end = this.cps[7];
        this.updateCp(1, start.x + (end.x - start.x) / 2, start.y);
        this.updateCp(2, end.x, start.y);
        this.updateCp(3, start.x, start.y + (end.y - start.y) / 2);
        this.updateCp(4, end.x, start.y + (end.y - start.y) / 2);
        this.updateCp(5, start.x, end.y);
        this.updateCp(6, start.x + (end.x - start.x) / 2, end.y);
    }

    validate() {
        super.validate();
        if (this.cps.length !== 8) {
            return false;
        }
        if (this.cps[0].equals(this.cps[7])) {
            return false;
        }
        return true;
    }

    handleMouseDown(evt) {
        console.log('[rectangle]handle MouseDown');
        super.handleMouseDown(evt);
        this.isMousedown = true;
        if (evt.target.type !== ShapeTypeEnum.rectangle
            && evt.target.type !== ShapeTypeEnum.controlpoint
            && evt.target.type !== ShapeTypeEnum.text) {
            this.updateCp(0, evt.offsetX, evt.offsetY);
        }
    }
    handleMouseMove(evt) {
        if (this.isMousedown) {
            console.log('[rectangle]handle MouseMove');
            this.isPaint = true;
            const evtPoint = new Point(evt.offsetX, evt.offsetY);
            if (this.utils.isRecInPolygon(this.cps[0], evtPoint, this.boundaryPts)) {
                this.updateCp(7, evt.offsetX, evt.offsetY);
                this.update();
            }
        }
    }
    handleMouseUp(evt) {
        console.log('[rectangle]handle MouseUp');
        this.isMousedown = false;
        this.isPaint = false;
        EventAggregator.Instance().contourReadyEvent.publish([[this.cps], this.contour_type, this.contour_uid]);
    }
    handlePressMove(evt) {
        console.log('[rectangle]handle PressMove');
        if (this.roiConfig.id === this.selectedRoi_id) {
            const evtPoint = new Point(evt.stageX, evt.stageY);
            if (this.utils.isInPolygon(evtPoint, this.boundaryPts)) {
                const tempX: number = this._tempPoint.x;
                const tempY: number = this._tempPoint.y;
                const delta_x = evt.stageX - tempX;
                const delta_y = evt.stageY - tempY;
                this.updateCpsByDelta(evt, delta_x, delta_y);
            }
        }
    }

    handlePressUp(evt) {
        if (this.roiConfig.id === this.selectedRoi_id) {
            super.handlePressUp(evt);
            EventAggregator.Instance().updateSigleContourEvent.publish([this.cps, this.contour_uid]);
        }
    }

    private updateCpsByDelta(evt, delta_x, delta_y) {
        const _tempCps0 = this.cps[0].copy();
        console.log('[_tempCps0] ' + _tempCps0.x + '  ' + _tempCps0.y);
        const _tempCps7 = this.cps[7].copy();
        switch (evt.target) {
            case this.top_left:
                _tempCps0.offset(delta_x, delta_y);
                break;
            case this.top_center:
                _tempCps0.offset(null, delta_y);
                break;
            case this.top_right:
                _tempCps0.offset(null, delta_y);
                _tempCps7.offset(delta_x, null);
                break;
            case this.left_center:
                _tempCps0.offset(delta_x, null);
                break;
            case this.right_center:
                _tempCps7.offset(delta_x, null);
                break;
            case this.bottom_left:
                _tempCps0.offset(delta_x, null);
                _tempCps7.offset(null, delta_y);
                break;
            case this.bottom_center:
                _tempCps7.offset(null, delta_y);
                break;
            case this.bottom_right:
                _tempCps7.offset(delta_x, delta_y);
                break;
            default:
                _tempCps0.offset(delta_x, delta_y);
                _tempCps7.offset(delta_x, delta_y);
                break;
        }
        if (this.utils.isRecInPolygon(_tempCps0, _tempCps7, this.boundaryPts)) {
            this.cps[0] = _tempCps0;
            this.cps[7] = _tempCps7;
            this._tempPoint.x = evt.stageX;
            this._tempPoint.y = evt.stageY;
            this.update();
        }
    }
}
