import { Directive, ElementRef, HostListener, Input, OnInit } from '@angular/core';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
import { KeyValuePair } from '../../../../shared/common/keyvaluepair';
import { CircleContainer } from '../container/circle_container';
import { FaderContainer } from '../container/fader_container';
import { FreepenContainer } from '../container/freepen_container';
import { LineContainer } from '../container/line_container';
import { RectangleContainer } from '../container/rectangle_container';
import { ROIConfig } from '../model/ROIConfig.model';
import { ConMessageService } from '../service/conMessage.service';
import { NudgeHelper } from '../tools/nudge_helper';
import { Point } from '../tools/point';

declare var createjs: any;
declare var actions: any;
declare var shapes: any;

@Directive({
    selector: '[overlay-canvas]'
})
export class OverlayCanvasDirective implements OnInit {
    curAction: string;
    actionInfo: KeyValuePair;
    canvasLeft: number;
    canvasTop: number;
    clickXs: Array<number> = Array<number>();
    clickYs: Array<number> = Array<number>();
    startX: number;
    startY: number;
    startPoint: Point;
    curX: number;
    curY: number;
    radius: number;
    contourColor: string = "#00ffff";
    contourLineWidth = 2;
    isFirstClick: boolean = true;
    myContext: CanvasRenderingContext2D;
    myStage: any;
    backStage: any;
    labelTxt: string;
    shape: any;
    fader: FaderContainer;
    nudgeHelper: NudgeHelper;
    curTarget: any;
    activeROI: ROIConfig;
    sliceIndex: any;
    preFaderPos: Point;
    @Input() backCanvas;
    @Input() name;

    constructor(private el: ElementRef, private contouringService: ConMessageService) { }

    ngOnInit() {
        console.log('[overlay-canvas]ngOnInit');
        this.myContext = this.el.nativeElement.getContext("2d");
        this.myStage = new createjs.Stage(this.el.nativeElement);
        this.actionInfo = new KeyValuePair(actions.locate);
        createjs.Touch.enable(this.myStage);
        this.myStage.enableMouseOver();
        this.myStage.mouseMoveOutside = true;
        this.myStage.autoClear = false;
        this.myStage.name = this.name;

        this.myContext.strokeStyle = this.contourColor;
        this.myContext.lineWidth = this.contourLineWidth;

        EventAggregator.Instance().actionInfo.subscribe(actionInfo => {
            if (actionInfo == null) {
                console.log('ActionInfo is wrong.');
                return;
            }
            console.log('[overlay-canvas]Current action is ' + actionInfo.key());
            if (actionInfo.key() == actions.clear) {
                if (this.myStage.children.length > 0) {
                    let roi_uid = this.activeROI.ROIId;
                    let slice_index = this.sliceIndex;
                    EventAggregator.Instance().removeCps.publish([roi_uid, slice_index])
                    this.myStage.removeAllChildren()
                    this.myStage.clear()
                }
            }
            this.actionInfo = actionInfo;
        })

        this.contouringService.graphics$.subscribe(data => {
            if (this.name != data[0]) return;
            this.myStage.removeAllChildren();
            this.myStage.clear();
            let graphics = data[1];
            let contours = [];
            if (graphics == null || graphics.length == 0) return;
            graphics.forEach(graphic => {
                if (graphic == null || graphic.length == 0) return;
                graphic.forEach(shape => {
                    let contour = []
                    shape.forEach(cp => {
                        contour.push(new Point(cp.x, cp.y))
                    });
                    contours.push(contour)
                });
            });
            // draw graphics
            contours.forEach(cps => {
                cps.push(cps[0])
                let freepen = new FreepenContainer(this.myStage)
                freepen.cps = cps
                freepen.update()
            });
        });

        this.contouringService.activeRoi$.subscribe(data => {
            this.activeROI = data;
        });

        this.contouringService.sliceIndex$.subscribe(data => {
            this.sliceIndex = data;
        });

        EventAggregator.Instance().scrollInfo.subscribe(data => {
            this.fader.updateRadius(data);
        });
    }

    @HostListener('mousedown', ['$event']) onMouseDown(event: MouseEvent) {
        console.log('[overlay-canvas]mousedown');
        this.myStage.children.forEach(shape => {
            if (shape.type === shapes.freepen) {
                shape.editable = this.actionInfo.value() === shapes.freepen_edit ? true : false;
            }
        });

        if (this.actionInfo.key() === actions.nudge) {
            this.fader = this.getFader();
            this.fader.handleMouseDown(event);
            this.preFaderPos = this.fader.getCenter();
            this.nudgeHelper.setMode(this.fader.getCenter(), this.getAllFreepenCps());
        }

        this.shape = this.getShapeContainerInstance();
        if (this.shape != null) {
            this.shape.setRoi(this.activeROI);
            this.shape.handleMouseDown(event);
        }
    }

    @HostListener('mousemove', ['$event']) onMouseMove(event: MouseEvent) {
        console.log('[overlay-canvas]mousemove');
        if (this.shape != null) {
            this.shape.handleMouseMove(event)
        }

        if (this.actionInfo.key() == actions.nudge) {
            this.fader = this.getFader();
            this.fader.handleMouseMove(event);
            if (this.fader.isMousedown) {
                let curFaderPos = this.fader.getCenter();
                let bridge = this.CreatBridgeRectCps(this.preFaderPos, curFaderPos);
                this.preFaderPos = curFaderPos;
                this.clip([bridge]);
            } else {
                this.nudgeHelper.setMode(this.fader.getCenter(), this.getAllFreepenCps());
                this.nudgeHelper.setState();
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

        let contours = [];
        this.myStage.children.forEach(contour => {
            if (contour.type === shapes.freepen) {
                contours.push(contour.cps);
            }
        });

        if (contours.length > 0) {
            let roi_uid = this.activeROI.ROIId;
            let slice_index = this.sliceIndex;
            EventAggregator.Instance().contourCps.publish([roi_uid, slice_index, contours])
        }
    }

    @HostListener('mouseleave', ['$event']) onMouseLeave(event: MouseEvent) {
        console.log('[overlay-canvas]mouseleave');
        this.myStage.removeChild(this.fader);
        this.fader = null;
        this.myStage.clear();
        this.myStage.update();
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
                return new LineContainer(this.myStage);
            case shapes.rectangle:
                return new RectangleContainer(this.myStage);
            case shapes.circle:
                return new CircleContainer(this.myStage);
            case shapes.freepen:
                return new FreepenContainer(this.myStage);
            case shapes.nudge:
                return new FaderContainer(this.myStage);
            default:
                return null;
        }
    }

    getFader() {
        if (this.fader == null) {
            this.fader = new FaderContainer(this.myStage);
            this.fader.setRoi(this.activeROI);
            this.nudgeHelper = new NudgeHelper(this.fader)
        }
        return this.fader;
    }

    clip(bridgeCps: Array<Array<Point>>) {
        let result = this.nudgeHelper.Push(this.getAllFreepenCps(), bridgeCps)

        this.removeAllFreepens();

        result.forEach(contour => {
            let cps = new Array();
            contour.forEach(cp => {
                cps.push(new Point(cp['X'], cp['Y']));
            });
            cps.push(cps[0].copy());
            let freepen = new FreepenContainer(this.myStage);
            freepen.setRoi(this.activeROI);
            this.myStage.addChild(freepen);
            freepen.setCps(cps);
            freepen.update();
        });
    }

    getAllFreepenCps(): Array<Array<Point>> {
        let cps = new Array();
        this.myStage.children.forEach(contour => {
            if (contour.type == shapes.freepen) {
                cps.push(contour.cps);
            }
        });
        return cps;
    }

    removeAllFreepens() {
        let freepens = [];
        this.myStage.children.forEach(contour => {
            if (contour.type === shapes.freepen) {
                freepens.push(contour);
            }
        });
        freepens.forEach(freepen => {
            this.myStage.removeChild(freepen);
        });
        this.myStage.clear();
        this.myStage.update();
    }

    private CreatBridgeRectCps(prePoint: Point, currPoint: Point) {
        let radius = this.fader.getRadius();
        let start = prePoint;
        let end = currPoint;
        if (start.equals(end)) {
            return null;
        }
        let vec = [end.x - start.x, end.y - start.y];
        let deno = Math.sqrt(vec[0] ** 2 + vec[1] ** 2);
        vec = [vec[0] / deno, vec[1] / deno];
        let crossVec = [-vec[1], vec[0]];

        // the rect
        let p1 = new Point(start.x + crossVec[0] * radius, start.y + crossVec[1] * radius);
        let p2 = new Point(end.x + crossVec[0] * radius, end.y + crossVec[1] * radius);
        let p3 = new Point(end.x - crossVec[0] * radius, end.y - crossVec[1] * radius);
        let p4 = new Point(start.x - crossVec[0] * radius, start.y - crossVec[1] * radius);
        return [p1, p2, p3, p4, p1.copy()];
    }
}