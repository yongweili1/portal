import { BaseContainer } from "./base_container";
import { Fader } from "../overlay/fader";
import { Point } from "../tools/point";
import { RoiModel } from "../model/roi.model";
import { Line } from "../overlay/line";

export class FaderContainer extends BaseContainer {
    fader: Fader;
    prePos: Point;
    horizontal: Line;
    vertical: Line;

    // none, none, 0
    // positive, +, 1
    // negative, -, -1
    state: number = 0;

    constructor(stage) {
        super(stage, 'nudge');
        this.fader = new Fader(stage);
        this.horizontal = new Line(stage);
        this.vertical = new Line(stage);
        this.addChild(this.horizontal, this.vertical, this.fader);
    }

    public setRoi(roi: RoiModel) {
        if (roi === undefined) return;
        super.setRoi(roi)
           this.fader.color = roi.ROIColor;
        this.horizontal.color = roi.ROIColor;
        this.vertical.color = roi.ROIColor;
    }

    update() {
        this.showState();
        this.fader.update(this.fader.center);
    }

    showState(state = 0) {
        if (state != 0) this.state = state;
        let p = this.getCenter().copy();
        this.horizontal.update(p.copy().offset(-this.getRadius() / 2, 0), p.copy().offset(this.getRadius() / 2, 0))
        if (this.state == 1) { // positive, +
            this.vertical.update(p.copy().offset(0, -this.getRadius() / 2), p.copy().offset(0, this.getRadius() / 2))
        } else { // negative, - or none
            this.vertical.update(this.getCenter(), this.getCenter())
        }
    }

    getCenter() {
        return this.fader.center;
    }

    getRadius() {
        return this.fader.radius;
    }

    updateRadius(delta: number) {
        this.fader.radius += delta;
        this.update();
    }

    getCps() {
        return this.fader.cps;
    }

    handleMouseDown(evt) {
        super.handleMouseDown(evt)

        if (evt.offsetX === undefined || evt.offsetY === undefined) {
            this.prePos = new Point(evt.stageX, evt.stageY)
        } else {
            this.prePos = new Point(evt.offsetX, evt.offsetY)
        }

        this.fader.setCenter(this.prePos);
        this.update();
    }

    handleMouseMove(e) {
        this.fader.setCenter(new Point(e.offsetX, e.offsetY));
        this.update();
    }

    handleMouseUp(e) {
        super.handleMouseUp(e);
        this.isPaint = false;
    }

    handlePressMove(e) {
    }

    handleMouseLeave(e) {
    }
}
