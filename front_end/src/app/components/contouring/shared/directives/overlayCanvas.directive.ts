import { Directive, ElementRef, HostListener, Input, OnChanges, OnInit, SimpleChanges, OnDestroy } from '@angular/core';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
import { KeyValuePair } from '../../../../shared/common/keyvaluepair';
import { CircleContainer } from '../container/circle_container';
import { FaderContainer } from '../container/fader_container';
import { FreepenContainer } from '../container/freepen_container';
import { LineContainer } from '../container/line_container';
import { RectangleContainer } from '../container/rectangle_container';
import { RoiModel } from '../model/roi.model';
import { NudgeHelper } from '../tools/nudge_helper';
import { Point } from '../tools/point';
import { Subscription } from 'rxjs';

declare var createjs: any;
declare var actions: any;
declare var shapes: any;

@Directive({
    selector: '[overlay-canvas]'
})
export class OverlayCanvasDirective implements OnInit, OnChanges, OnDestroy {
    context: CanvasRenderingContext2D;
    stage: any;
    shape: any;
    fader: FaderContainer;
    nudgeHelper: NudgeHelper;
    preFaderPos: Point;
    rois: Array<RoiModel>;
    roi: RoiModel;
    graphicChanged = true;

    roisSubscriber: Subscription;
    roiSubscriber: Subscription;

    @Input() faderRadius: number;
    @Input() sliceIndex: any;
    @Input() tag;
    @Input() graphics;
    @Input() actionInfo: KeyValuePair;

    constructor(private el: ElementRef) { }

    ngOnInit() {
        console.log('[overlay-canvas]ngOnInit');
        this.context = this.el.nativeElement.getContext('2d');
        this.stage = new createjs.Stage(this.el.nativeElement);
        this.actionInfo = new KeyValuePair(actions.locate);
        createjs.Touch.enable(this.stage);
        this.stage.enableMouseOver();
        this.stage.mouseMoveOutside = true;
        this.stage.autoClear = false;
        this.stage.name = this.tag;

        this.roisSubscriber = EventAggregator.Instance().rois.subscribe(rois => {
            this.rois = rois;

            // delete exilic contours
            const exilicContours = [];
            this.stage.children.forEach(contour => {
                if (contour.roiConfig === undefined) {
                    return;
                }
                const roiuid = contour.roiConfig.id;
                if (contour.type === shapes.freepen && this.rois.findIndex(x => x.id === roiuid) === -1) {
                    exilicContours.push(contour);
                }
            });
            exilicContours.forEach(freepen => {
                this.stage.removeChild(freepen);
            });
            this.stage.clear();
            this.stage.update();
        });

        this.roiSubscriber = EventAggregator.Instance().roi.subscribe(roi => {
            this.roi = roi;
        });
    }

    ngOnChanges(changes: SimpleChanges) {
        if (changes.actionInfo !== undefined) {
            console.log('[overlay-canvas]Current action is ' + this.actionInfo.key());
            if (this.actionInfo.key() === actions.clear) {
                if (this.stage.children.length > 0) {
                    const roi_uid = this.roi.id;
                    const slice_index = this.sliceIndex;
                    EventAggregator.Instance().removeCps.publish([roi_uid, slice_index]);
                    this.stage.removeAllChildren();
                    this.stage.clear();
                }
            }
        }

        if (changes.graphics !== undefined) {
            this.update();
        }

        if (changes.faderRadius !== undefined) {
            if (this.fader !== null && this.fader !== undefined) {
                this.fader.setRadius(this.faderRadius);
                this.fader.update();
            }
        }
    }

    ngOnDestroy() {
        try {
            this.roisSubscriber.unsubscribe();
            this.roiSubscriber.unsubscribe();
        } catch (error) {
            console.error(error.message);
        }
    }

    @HostListener('mousedown', ['$event']) onMouseDown(event: MouseEvent) {
        console.log('[overlay-canvas]mousedown');
        this.stage.children.forEach(shape => {
            if (shape.type === shapes.freepen) {
                shape.editable = this.actionInfo.value() === shapes.freepen_edit ? true : false;
            }
        });

        if (this.actionInfo.key() === actions.nudge) {
            this.fader = this.getFader();
            this.fader.handleMouseDown(event);
            this.preFaderPos = this.fader.getCenter();
            this.nudgeHelper.setMode(this.fader.getCenter(), this.getFreepenCps(this.roi.id));
        }

        this.shape = this.getShapeContainerInstance();
        if (this.shape != null) {
            this.shape.setRoi(this.roi);
            this.shape.handleMouseDown(event);
        }
    }

    @HostListener('mousemove', ['$event']) onMouseMove(event: MouseEvent) {
        if (this.shape != null) {
            this.shape.handleMouseMove(event);
        }

        if (this.actionInfo.key() === actions.nudge) {
            this.fader = this.getFader();
            this.fader.handleMouseMove(event);
            if (this.fader.isMousedown) {
                const curFaderPos = this.fader.getCenter();
                const bridge = this.CreatBridgeRectCps(this.preFaderPos, curFaderPos);
                this.preFaderPos = curFaderPos;
                this.clip([bridge]);
            } else {
                if (this.roi !== undefined) {
                    if (this.fader.roiConfig === undefined) {
                        this.fader.setRoi(this.roi);
                    }
                    this.nudgeHelper.setMode(this.fader.getCenter(), this.getFreepenCps(this.roi.id));
                    this.nudgeHelper.setState();
                }
            }
        }
    }

    @HostListener('mouseup', ['$event']) onMouseUp(event: MouseEvent) {
        console.log('[overlay-canvas]mouseup');
        if (this.shape != null) {
            this.shape.handleMouseUp(event);
        }

        if (this.actionInfo.key() === actions.nudge) {
            this.fader = this.getFader();
            this.fader.handleMouseUp(event);
        }

        const contours = this.getFreepenCps(this.roi.id);
        if (contours.length > 0 && this.graphicChanged) {
            const roi_uid = this.roi.id;
            const slice_index = this.sliceIndex;
            EventAggregator.Instance().contourCps.publish([roi_uid, slice_index, contours]);
        }
    }

    @HostListener('mouseleave', ['$event']) onMouseLeave(event: MouseEvent) {
        console.log('[overlay-canvas]mouseleave');
        this.stage.removeChild(this.fader);
        this.fader = null;
        this.stage.clear();
        this.stage.update();
    }

    @HostListener('dblclick', ['$event']) onDbClick(event: MouseEvent) {
        console.log('[overlay-canvas]dblclick');
    }

    getShapeContainerInstance() {
        if (this.actionInfo.key() !== actions.shape) {
            console.log('Can not create shape on this action');
            return null;
        }
        switch (this.actionInfo.value()) {
            case shapes.line:
                return new LineContainer(this.stage);
            case shapes.rectangle:
                return new RectangleContainer(this.stage);
            case shapes.circle:
                return new CircleContainer(this.stage);
            case shapes.freepen:
                return new FreepenContainer(this.stage);
            case shapes.nudge:
                return new FaderContainer(this.stage);
            default:
                return null;
        }
    }

    getFader() {
        if (this.fader == null || this.fader === undefined) {
            this.fader = new FaderContainer(this.stage);
            this.fader.setRoi(this.roi);
            this.fader.setRadius(this.faderRadius);
            this.nudgeHelper = new NudgeHelper(this.fader);
        }
        return this.fader;
    }

    clip(bridgeCps: Array<Array<Point>>) {
        const result = this.nudgeHelper.Push(this.getFreepenCps(this.roi.id), bridgeCps);

        this.removeFreepens(this.roi.id);

        result.forEach(contour => {
            const cps = new Array();
            contour.forEach(cp => {
                cps.push(new Point(cp['X'], cp['Y']));
            });
            cps.push(cps[0].copy());
            const freepen = new FreepenContainer(this.stage);
            freepen.setRoi(this.roi);
            this.stage.addChild(freepen);
            freepen.setCps(cps);
            freepen.update();
        });
    }

    getFreepenCps(roiuid): Array<Array<Point>> {
        const cps = new Array();
        this.stage.children.forEach(contour => {
            if (contour.type === shapes.freepen
                && contour.roiConfig !== undefined
                && contour.roiConfig.id === roiuid) {
                cps.push(contour.cps);
            }
        });
        return cps;
    }

    removeFreepens(roiuid) {
        const freepens = [];
        this.stage.children.forEach(contour => {
            if (contour.type === shapes.freepen && contour.roiConfig.id === roiuid) {
                freepens.push(contour);
            }
        });
        freepens.forEach(freepen => {
            this.stage.removeChild(freepen);
        });
        this.stage.clear();
        this.stage.update();
    }

    private CreatBridgeRectCps(prePoint: Point, currPoint: Point) {
        const radius = this.fader.getRadius();
        const start = prePoint;
        const end = currPoint;
        if (start.equals(end)) {
            return null;
        }
        let vec = [end.x - start.x, end.y - start.y];
        const deno = Math.sqrt(vec[0] ** 2 + vec[1] ** 2);
        vec = [vec[0] / deno, vec[1] / deno];
        const crossVec = [-vec[1], vec[0]];

        // the rect
        const p1 = new Point(start.x + crossVec[0] * radius, start.y + crossVec[1] * radius);
        const p2 = new Point(end.x + crossVec[0] * radius, end.y + crossVec[1] * radius);
        const p3 = new Point(end.x - crossVec[0] * radius, end.y - crossVec[1] * radius);
        const p4 = new Point(start.x - crossVec[0] * radius, start.y - crossVec[1] * radius);
        return [p1, p2, p3, p4, p1.copy()];
    }

    update() {
        if (this.stage === undefined) {
            return;
        }
        // if (this.name != data[0]) return;
        this.stage.removeAllChildren();
        this.stage.clear();
        const contours = [];
        if (this.graphics === undefined || this.graphics.length === 0) {
            return;
        }
        this.graphics.forEach(graphic => {
            if (graphic === undefined || graphic.length === 0) {
                return;
            }
            const contour = [];
            graphic.cps.forEach(cp => {
                contour.push(new Point(cp.x, cp.y));
            });
            contours.push([graphic.roiuid, contour]);
        });
        // draw graphics
        contours.forEach(contour => {
            const roiuid = contour[0];
            const cps = contour[1];
            cps.push(cps[0].copy());
            const freepen = new FreepenContainer(this.stage);
            freepen.setRoi(this.rois.find(x => x.id === roiuid));
            freepen.cps = cps;
            freepen.update();
        });
    }
}
