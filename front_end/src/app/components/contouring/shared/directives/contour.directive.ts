import { Directive, ElementRef, Input, HostListener, OnInit, Output, EventEmitter } from '@angular/core';
import { ROIConfig } from '../model/ROIConfig.model'
import { ConMessageService } from '../service/conMessage.service';
import { CircleFactory } from '../tools/factory/circle-factory'
import { LineFactory } from '../tools/factory/line-factory'
import { RectangleFactory } from '../tools/factory/rectangle-factory'
import { FreepenFactory } from '../tools/factory/freepen-factory'
import { Point } from '../tools/point'
import { FreepenContainer } from '../container/freepen_container';
import { KeyValuePair } from '../../../../shared/common/keyvaluepair';
import { FaderFactory } from '../tools/factory/fader-factory';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
import { FaderContainer } from '../container/fader_container';
import { NudgeHelper } from '../tools/nudge_helper';

declare var createjs: any;
declare var actions: any;
declare var shapes: any;

@Directive({
    selector: '[myContour]'
})
export class ContourDirective implements OnInit {
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
    activeROI:ROIConfig;
    preFaderPos: Point;
    @Input() backCanvas;
    @Input() name;

    constructor(private el: ElementRef, private contouringService: ConMessageService) { }

    ngOnInit() {
        this.myContext = this.el.nativeElement.getContext("2d");
        this.myStage = new createjs.Stage(this.el.nativeElement);
        this.actionInfo = new KeyValuePair(actions.locate, null);
        createjs.Touch.enable(this.myStage);
        this.myStage.enableMouseOver();
        this.myStage.mouseMoveOutside = true;
        this.myStage.autoClear = false;
        this.myStage.name = this.name;

        this.myContext.strokeStyle = this.contourColor;
        this.myContext.lineWidth = this.contourLineWidth;

        this.contouringService.actionInfo$.subscribe(actionInfo => {
            if (actionInfo == null) {
                console.log('ActionInfo is wrong.')
                return;
            }
            console.log('Current action is ' + actionInfo.key());
            if (actionInfo.key() == actions.clear) {
                this.myStage.removeAllChildren()
                this.myStage.clear()
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
                        contour.push(new Point(cp[0][0], cp[0][1]))
                    });
                    contours.push(contour)
                });
            });
            // draw graphics
            contours.forEach(cps => {
                cps.push(cps[0].copy())
                let freepen = new FreepenContainer(this.myStage)
                freepen.cps = cps
                freepen.update()
            });
        });

        this.contouringService.activeRoi$.subscribe(data=>{
            this.activeROI = data;
        })
    }

    @HostListener('mousedown', ['$event']) onMouseDown(event: MouseEvent) {
        this.myStage.children.forEach(shape => {
            if (shape.type == shapes.freepen) {
                shape.editable = this.actionInfo.value() == shapes.freepen_edit ? true : false;
            }
        });

        if (this.actionInfo.key() == actions.nudge) {
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
                let mode = this.nudgeHelper.setMode(this.fader.getCenter(), this.getAllFreepenCps());
                if (mode == 'CreateInFader' || mode == 'InFader' || mode == 'CreateOutFader') {
                    this.fader.showState(1);
                } else if (mode == 'OutFader') {
                    this.fader.showState(-1);
                }
            }
        }
    }

    @HostListener('mouseup', ['$event']) onMouseUp(event: MouseEvent) {
        if (this.shape != null)
            this.shape.handleMouseUp(event)

        if (this.actionInfo.key() == actions.nudge) {
            this.fader = this.getFader();
            this.fader.handleMouseUp(event);
        }
    }

    @HostListener('mouseleave', ['$event']) onMouseLeave(event: MouseEvent) {
        this.onMouseUp(event);
        this.myStage.removeChild(this.fader);
        this.fader = null;
        this.myStage.clear();
        this.myStage.update()
    }

    @HostListener('dblclick', ['$event']) onDbClick(event: MouseEvent) { }

    getShapeContainerInstance() {
        if (this.actionInfo.key() != actions.shape) {
            console.log('Can not create shape on this action')
            return null;
        }
        switch(this.actionInfo.value()) {
            case shapes.line:
                return LineFactory.getInstance().createSharpContainer(this.myStage);
            case shapes.rectangle:
                return RectangleFactory.getInstance().createSharpContainer(this.myStage);
            case shapes.circle:
                return CircleFactory.getInstance().createSharpContainer(this.myStage);
            case shapes.freepen:
                return FreepenFactory.getInstance().createSharpContainer(this.myStage);
            case shapes.nudge:
                return FaderFactory.getInstance().createSharpContainer(this.myStage);
            default:
                return null;
        }
    }

    getFader() {
        if (this.fader == null) {
            this.fader = FaderFactory.getInstance().createSharpContainer(this.myStage);
            this.fader.setRoi(this.activeROI);
            this.nudgeHelper = new NudgeHelper(this.fader)
        }
        return this.fader;
    }

    clip(bridgeCps: Array<Array<Point>>) {
        let result = this.nudgeHelper.Push(this.getAllFreepenCps(), bridgeCps)

        this.removeAllFreepens()

        result.forEach(contour => {
            let cps = new Array()
            contour.forEach(cp => {
                cps.push(new Point(cp['X'], cp['Y']))
            });
            cps.push(cps[0].copy())
            let freepen = FreepenFactory.getInstance().createSharpContainer(this.myStage);
            freepen.setRoi(this.activeROI);
            this.myStage.addChild(freepen)
            freepen.setCps(cps)
            freepen.update()
        });
    }

    getAllFreepenCps():Array<Array<Point>> {
        let cps = new Array();
        this.myStage.children.forEach(contour => {
            if (contour.type == shapes.freepen) {
                cps.push(contour.cps)
            }
        });
        return cps;
    }

    removeAllFreepens() {
        let freepens = []
        this.myStage.children.forEach(contour => {
            if (contour.type == shapes.freepen) {
                freepens.push(contour)
            }
        });
        freepens.forEach(freepen => {
            this.myStage.removeChild(freepen)
        });
        this.myStage.clear()
        this.myStage.update()
    }

    private CreatBridgeRectCps(prePoint: Point, currPoint:Point) {
        let radius = this.fader.getRadius()
        let start = prePoint;
        let end = currPoint;
        if (start.equals(end))
        {
            return null;
        }
        let vec = [end.x - start.x, end.y - start.y];
        let deno = Math.sqrt(vec[0] ** 2 + vec[1] ** 2);
        vec = [vec[0] / deno, vec[1] / deno];
        let crossVec = [-vec[1], vec[0]];

        //the rect
        let p1 = new Point(start.x + crossVec[0] * radius, start.y + crossVec[1] * radius)
        let p2 = new Point(end.x + crossVec[0] * radius, end.y + crossVec[1] * radius)
        let p3 = new Point(end.x - crossVec[0] * radius, end.y - crossVec[1] * radius)
        let p4 = new Point(start.x - crossVec[0] * radius, start.y - crossVec[1] * radius)
        return [p1, p2, p3, p4, p1.copy()];
    }
}