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
import { ClipperHelper, ClipPoint } from '../tools/clipper_helper';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
import { FaderContainer } from '../container/fader_container';

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
    curTarget: any;
    activeROI:ROIConfig;
    @Input() backCanvas;
    @Input() name;
    _faderMode: string;

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
            if (this.fader == null)
                this.fader = FaderFactory.getInstance().createSharpContainer(this.myStage);
            this.fader.handleMouseDown(event);
            this._faderMode = this.FaderMode(this.fader.getCenter(), this.getAllFreepenCps());
            console.log(this._faderMode);
        }

        this.shape = this.getShapeContainerInstance();
        
        if (this.shape != null) {
            this.shape.roiConfig = this.activeROI;
            this.shape.handleMouseDown(event);
        }
    }

    @HostListener('mousemove', ['$event']) onMouseMove(event: MouseEvent) {
        if (this.shape != null) {
            this.shape.handleMouseMove(event)
        }
        
        if (this.actionInfo.key() == actions.nudge) {
            if (this.fader == null)
                this.fader = FaderFactory.getInstance().createSharpContainer(this.myStage);
            this.fader.handleMouseMove(event);
            if (this.fader.isMousedown) {
                this.clip();
            }
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

    PointInPoly(pt, poly) {
        for (var c = false, i = -1, l = poly.length, j = l - 1; ++i < l; j = i)
            ((poly[i].y <= pt.y && pt.y < poly[j].y) || (poly[j].y <= pt.y && pt.y < poly[i].y))
            && (pt.x < (poly[j].x - poly[i].x) * (pt.y - poly[i].y) / (poly[j].y - poly[i].y) + poly[i].x)
            && (c = !c);
        return c;
    }

    clip() {
        let result = this.Pusher()

        this.removeAllFreepens()

        result.forEach(contour => {
            let cps = new Array()
            contour.forEach(cp => {
                cps.push(new Point(cp['X'], cp['Y']))
            });
            cps.push(cps[0].copy())
            let freepen = FreepenFactory.getInstance().createSharpContainer(this.myStage);
            this.myStage.addChild(freepen)
            freepen.setCps(cps)
            freepen.update()
        });
    }

    getAllFreepens() {
        let freepens = new Array();
        this.myStage.children.forEach(contour => {
            if (contour.type == shapes.freepen) {
                freepens.push(contour)
            }
        });
        return freepens;
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
        this.myStage.children.forEach(contour => {
            if (contour.type == shapes.freepen) {
                this.myStage.removeChild(contour)
            }
        });
    }

    convertPoints(contours: Array<Array<Point>>) {
        let rois = new Array();
        contours.forEach(contour => {
            if (contour == null) return;
            let roi = new Array();
            contour.forEach(cp => {
                let p = new ClipPoint();
                p.X = cp.x;
                p.Y = cp.y;
                roi.push(p)
            });
            rois.push(roi);
        });
        return rois;
    }

    private FaderMode(center: Point, voiPt: Array<Array<Point>>): string {
        let voiPt_Clipper = this.convertPoints(voiPt);
        let pointInContour: boolean = this.ContourContainsPoint(voiPt_Clipper, { X: center.x, Y: center.y });
        let virtualFaderPt_Clipper = this.convertPoints([this.fader.getCps()])
        if (pointInContour) {
            let intersection = ClipperHelper.Clipper(voiPt_Clipper, virtualFaderPt_Clipper, 1, "intersection");
            if (intersection[0].length == this.fader.getCps().length) {
                return "CreateInFader";
            } else {
                return "InFader";
            }
        } else {
            let intersection = ClipperHelper.Clipper(voiPt_Clipper, virtualFaderPt_Clipper, 1, "intersection");
            if (intersection.length == 0) {
                return "CreateOutFader";
            } else {
                return "OutFader";
            }
        }
    } 

    private ContourContainsPoint(contours: Array<Array<ClipPoint>>, point: ClipPoint): boolean {
        let num: number = 0;
        for (var contour in contours) {
            let wn: number = 0, j: number = 0;
            for (let i = 0; i < contours[contour].length; i++) {
                if (i == contours[contour].length - 1) {
                    j = 0;
                } else {
                    j = j + 1;
                }

                if (contours[contour][i].Y <= point.Y) {   // 如果多边形的点 小于等于 选定点的 Y 坐标  
                    if (contours[contour][j].Y > point.Y) {  // 如果多边形的下一点 大于于 选定点的 Y 坐标  
                        if (this.IsLeft(contours[contour][i], contours[contour][j], point) > 0) {
                            wn++;
                        }
                    }
                } else {
                    if (contours[contour][j].Y <= point.Y) {
                        if (this.IsLeft(contours[contour][i], contours[contour][j], point) < 0) {
                            wn--;
                        }
                    }
                }
            }
            if (wn != 0) { num++; }
        }
        if (num % 2 == 0) {
            return false;
        } else {
            return true;
        }
    }

    private IsLeft(P0: ClipPoint, P1: ClipPoint, P2: ClipPoint): number {
        let abc: number = ((P1.X - P0.X) * (P2.Y - P0.Y) - (P2.X - P0.X) * (P1.Y - P0.Y));
        return abc;
    }

    private Pusher() {
        if (this.fader.getCps().length == 0 || this._faderMode == null) {
            return null;
        }

        let voiPt_Clipper = this.convertPoints(this.getAllFreepenCps());
        let pusherPt_Clipper = this.convertPoints([this.fader.getCps()]);
        if (this._faderMode == "InFader") {
            return this.InPusher(voiPt_Clipper, pusherPt_Clipper);
        } else if (this._faderMode == "OutFader") {
            return this.OutPusher(voiPt_Clipper, pusherPt_Clipper);
        } else if (this._faderMode == "CreateOutFader") {
            return this.OutPusher(voiPt_Clipper, pusherPt_Clipper);
        } else if (this._faderMode == "CreateInFader") {
            return this.InPusher(voiPt_Clipper, pusherPt_Clipper);
        }
    }

    private OutPusher(voiPt_Clipper: Array<Array<ClipPoint>>, pusherPt_Clipper: Array<Array<ClipPoint>>): Array<Array<number>> {
        let difference = [];
        difference = ClipperHelper.Clipper(voiPt_Clipper, pusherPt_Clipper, 1, "difference");
        return difference;
    }

    private InPusher(voiPt_Clipper: Array<Array<ClipPoint>>, pusherPt_Clipper: Array<Array<ClipPoint>>): Array<Array<number>> {
        let union = [];
        union = ClipperHelper.Clipper(voiPt_Clipper, pusherPt_Clipper, 1, "union");
        return union;
    }
}