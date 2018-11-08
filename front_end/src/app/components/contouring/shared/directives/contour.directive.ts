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
    fader: any;
    curTarget: any;
    activeROI:ROIConfig;
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
            this.myStage.removeAllChildren()
            this.myStage.clear()
            let graphics = data[1]
            let contours = []
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
        }
        )
    }

    @HostListener('mousedown', ['$event']) onMouseDown(event: MouseEvent) {
        this.myStage.children.forEach(shape => {
            if (shape.type == shapes.freepen) {
                shape.editable = this.actionInfo.value() == shapes.freepen_edit ? true : false;
            }
        });
        
        if (this.actionInfo.key() == actions.nudge) {
            if (this.fader == null)
                this.fader = FaderFactory.getInstance().createSharpContainer(this.myStage);
            this.fader.handleMouseDown(event)
        }

        this.shape = this.getShapeContainerInstance();
        this.shape.roiConfig = this.activeROI;
        this.shape.getChildAt(0).color = this.shape.roiConfig.ROIColor;
        if (this.shape != null)
            this.shape.handleMouseDown(event)
    }

    @HostListener('mousemove', ['$event']) onMouseMove(event: MouseEvent) {
        if (this.shape != null) {
            this.shape.handleMouseMove(event)
        }
        
        if (this.actionInfo.key() == actions.nudge) {
            if (this.fader == null)
                this.fader = FaderFactory.getInstance().createSharpContainer(this.myStage);
            this.fader.handleMouseMove(event);
        }
    }

    @HostListener('mouseup', ['$event']) onMouseUp(event: MouseEvent) {
        if (this.shape != null)
            this.shape.handleMouseUp(event)
        
            if (this.actionInfo.key() == actions.nudge) {
            if (this.fader == null)
                this.fader = FaderFactory.getInstance().createSharpContainer(this.myStage);
            this.fader.handleMouseUp(event);
        }
    }

    @HostListener('mouseleave', ['$event']) onMouseLeave(event: MouseEvent) {
        this.onMouseUp(event);
        if (this.fader != null) {
            this.fader.handleMouseLeave(event);
        }
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

    PointInPoly(pt, poly) {
        for (var c = false, i = -1, l = poly.length, j = l - 1; ++i < l; j = i) 
            ((poly[i].y <= pt.y && pt.y < poly[j].y) || (poly[j].y <= pt.y && pt.y < poly[i].y)) 
            && (pt.x < (poly[j].x - poly[i].x) * (pt.y - poly[i].y) / (poly[j].y - poly[i].y) + poly[i].x) 
            && (c = !c); 
        return c; 
    }
}