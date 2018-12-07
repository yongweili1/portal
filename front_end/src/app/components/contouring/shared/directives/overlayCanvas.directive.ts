import { Directive, ElementRef, HostListener, Input, OnChanges, OnInit, SimpleChanges, OnDestroy } from '@angular/core';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
import { CircleContainer } from '../container/circle_container';
import { FaderContainer } from '../container/fader_container';
import { FreepenContainer } from '../container/freepen_container';
import { LineContainer } from '../container/line_container';
import { RectangleContainer } from '../container/rectangle_container';
import { RoiModel } from '../model/roi.model';
import { NudgeHelper } from '../tools/nudge_helper';
import { Point } from '../tools/point';
import { ActionTypeEnum, ShapeTypeEnum } from '../../../../shared/models/enums';
import { BorderContainer } from '../container/border_container';
import { Utils } from '../tools/utils';

declare var createjs: any;


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
    graphicChanged = true;
    isMouseDown = false;
    utils = new Utils();

    @Input() rois: any;
    @Input() roi: RoiModel;
    @Input() faderRadius: number;
    @Input() sliceIndex: any;
    @Input() tag;
    @Input() graphics;
    @Input() actionType: ActionTypeEnum;
    @Input() shapeType: ShapeTypeEnum;
    @Input() boundaryPts: any = new Array<Point>();

    constructor(private el: ElementRef) { }

    ngOnInit() {
        console.log('[overlay-canvas]ngOnInit');
        this.context = this.el.nativeElement.getContext('2d');
        this.stage = new createjs.Stage(this.el.nativeElement);
        this.actionType = ActionTypeEnum.locate;
        createjs.Touch.enable(this.stage);
        this.stage.enableMouseOver();
        this.stage.mouseMoveOutside = true;
        this.stage.autoClear = false;
        this.stage.name = this.tag;

        EventAggregator.Instance().clearGraphicsEvent.subscribe(() => {
            if (this.stage.children.length > 0) {
                const roi_uid = this.roi.id;
                const slice_index = this.sliceIndex;
                EventAggregator.Instance().removeCps.publish([roi_uid, slice_index]);
                this.stage.removeAllChildren();
                this.stage.clear();
            }
        });
    }

    ngOnChanges(changes: SimpleChanges) {
        if (changes.currentShape !== undefined) {
            console.log('[overlay-canvas]Current shape is ' + this.shapeType);
        }

        if (changes.graphics !== undefined) {
            this.update();
        }

        if (changes.faderRadius !== undefined) {
            if (this.fader !== null && this.fader !== undefined) {
                this.fader.setRadius(this.faderRadius);
                this.fader.update();
                this.fader.setBoundaryPts(this.boundaryPts);
                this.updateBoundry();
            }
        }

        if (changes.rois !== undefined) {
            console.log('rois changed');
            // delete exilic contours
            if (this.stage !== undefined) {
                const exilicContours = [];
                this.stage.children.forEach(contour => {
                    if (contour.roiConfig === undefined) {
                        return;
                    }
                    const roiuid = contour.roiConfig.id;
                    if (contour.type === ShapeTypeEnum.freepen && this.rois.findIndex(x => x.id === roiuid) === -1) {
                        exilicContours.push(contour);
                    }
                });
                exilicContours.forEach(freepen => {
                    this.stage.removeChild(freepen);
                });
                this.stage.clear();
                this.stage.update();
            }
        }

        if (changes.roi !== undefined) {
            console.log('roi changed');
        }
    }

    ngOnDestroy() {
        try {
            console.log('OnDestroy');
        } catch (error) {
            console.error(error.message);
        }
    }

    @HostListener('mousedown', ['$event']) onMouseDown(event: MouseEvent) {
        console.log('[overlay-canvas]handle mousedown event');

        if (!this.utils.isInPolygon(new Point(event.offsetX, event.offsetY), this.boundaryPts)) {
            console.log('out of image');
            return;
        }

        this.isMouseDown = true;

        this.stage.children.forEach(shape => {
            if (shape.type === ShapeTypeEnum.freepen) {
                if (this.actionType === ActionTypeEnum.select) {
                    shape.editable = false;
                } else if (this.shapeType === ShapeTypeEnum.freepen2) {
                    shape.editable = true;
                } else {
                    shape.editable = false;
                }
            }
        });

        if (this.actionType === ActionTypeEnum.nudge) {
            this.fader = this.getFader();
            this.fader.handleMouseDown(event);
            this.preFaderPos = this.fader.getCenter();
            this.nudgeHelper.setMode(this.fader.getCenter(), this.getFreepenCps(this.roi.id));
        }

        this.shape = this.getShapeContainerInstance();
        if (this.shape != null) {
            this.shape.setRoi(this.roi);
            this.shape.handleMouseDown(event);
            this.shape.setBoundaryPts(this.boundaryPts);
        }
    }

    @HostListener('mousemove', ['$event']) onMouseMove(event: MouseEvent) {
        if (this.actionType === ActionTypeEnum.nudge) {
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

        if (this.shape != null && this.isMouseDown) {
            this.shape.handleMouseMove(event);
        }
    }

    @HostListener('mouseup', ['$event']) onMouseUp(event: MouseEvent) {
        console.log('[overlay-canvas]handle mouseup event');

        if (this.shape != null && this.isMouseDown) {
            this.shape.handleMouseUp(event);
        }
        this.isMouseDown = false;

        if (this.actionType === ActionTypeEnum.nudge) {
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
        if (this.actionType !== ActionTypeEnum.shape) {
            return null;
        }
        switch (this.shapeType) {
            case ShapeTypeEnum.line:
                return new LineContainer(this.stage);
            case ShapeTypeEnum.rectangle:
                return new RectangleContainer(this.stage);
            case ShapeTypeEnum.circle:
                return new CircleContainer(this.stage);
            case ShapeTypeEnum.freepen:
                return new FreepenContainer(this.stage);
            case ShapeTypeEnum.fader:
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
            this.fader.setBoundaryPts(this.boundaryPts);
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
            if (contour.type === ShapeTypeEnum.freepen
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
            if (contour.type === ShapeTypeEnum.freepen && contour.roiConfig.id === roiuid) {
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

        this.stage.removeAllChildren();
        this.stage.clear();

        this.updateBoundry();

        const contours = [];
        if (this.graphics === undefined || this.graphics.length === 0) {
            return;
        }
        this.graphics.forEach(graphic => {
            if (graphic === undefined || graphic.length === 0 || graphic.cps.length === 0) {
                return;
            }
            const contour = [];
            graphic.cps.forEach(cp => {
                contour.push(new Point(cp[0], cp[1]));
            });
            contours.push([graphic.roiuid, contour]);
        });
        // draw graphics
        if (contours.length === 0) {
            return;
        }
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

    updateBoundry() {
        if (this.boundaryPts === undefined || this.boundaryPts == null || this.boundaryPts.length !== 5) {
            return;
        }
        this._removeBorders();
        const border = new BorderContainer(this.stage);
        border.setCps(this.boundaryPts);
        border.setRoi(this.roi);
        border.update();
        const activeAreaBoundary = this.utils.scaleRectangleBoundary(this.boundaryPts, -this.faderRadius);
        const border1 = new BorderContainer(this.stage);
        border1.setCps(activeAreaBoundary);
        border1.setRoi(this.roi);
        border1.update();
    }

    private _removeBorders() {
        const borders = [];
        this.stage.children.forEach(child => {
            if (child.type === ShapeTypeEnum.border) {
                borders.push(child);
            }
        });
        borders.forEach(border => {
            this.stage.removeChild(border);
        });
        this.stage.clear();
        this.stage.update();
    }
}
