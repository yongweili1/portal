import { Directive, ElementRef, HostListener, Input, OnChanges, OnDestroy, OnInit, SimpleChanges } from '@angular/core';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
import { ActionTypeEnum, ShapeTypeEnum, ContourTypeEnum } from '../../../../shared/models/enums';
import { BorderContainer } from '../container/border_container';
import { CircleContainer } from '../container/circle_container';
import { FaderContainer } from '../container/fader_container';
import { FreepenContainer } from '../container/freepen_container';
import { LineContainer } from '../container/line_container';
import { RectangleContainer } from '../container/rectangle_container';
import { RoiModel } from '../model/roi.model';
import { NudgeHelper } from '../tools/nudge_helper';
import { Point } from '../tools/point';
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
    isMouseDown = false;
    utils = new Utils();
    _shapeType: ShapeTypeEnum = ShapeTypeEnum.freepen;

    @Input() rois: any;
    @Input() roi: RoiModel;
    @Input() faderRadius: number;
    @Input() sliceIndex: any;
    @Input() tag = 'transverse';
    @Input() graphics;
    @Input() actionType: ActionTypeEnum;
    // @Input() shapeType: ShapeTypeEnum;
    @Input() boundaryPts: any = new Array<Point>();
    @Input() fillGraphic: boolean;
    @Input()
    public set shapeType(_shape: ShapeTypeEnum) {
        if (_shape === undefined) { return; }
        if (this._shapeType !== _shape) {
            this._shapeType = _shape;
            if (this.stage === undefined || this.stage.children === undefined) { return; }
            this.stage.children.forEach(contour => {
                if (contour.type === ShapeTypeEnum.freepen) {
                    if (_shape === ShapeTypeEnum.freepen2 && this.actionType !== ActionTypeEnum.select) {
                        contour.editable = true;
                    } else {
                        contour.editable = false;
                    }
                }
            });
        }
    }
    public get shapeType() {
        return this._shapeType;
    }

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
                EventAggregator.Instance().deleteContoursEvent.publish([roi_uid, slice_index]);
                this.stage.removeAllChildren();
                this.stage.clear();
            }
        });

        EventAggregator.Instance().contourReadyEvent.subscribe((data) => {
            if (data.length === 3) {
                EventAggregator.Instance().saveSigleContourEvent.publish(data.concat(
                    [this.roi.id, this.sliceIndex, this.tag]));
            } else if (data === ContourTypeEnum.freepen || data === ContourTypeEnum.fader) {
                // EventAggregator.Instance().updateSigleContourEvent.publish(data.push(this.tag));
                if (this.tag === 'transverse') {

                    const contours = this.getFreepenCps(this.roi.id);
                    if (contours.length >= 0) {
                        const roi_uid = this.roi.id;
                        const slice_index = this.sliceIndex;
                        EventAggregator.Instance().saveContoursEvent.publish([contours, ContourTypeEnum.freepen, roi_uid, slice_index]);
                    }
                }

            } else {
                console.log('contour data error');
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
                // this.updateBoundry();
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

        if (changes.fillGraphic !== undefined && this.stage !== undefined && this.stage.children !== undefined) {
            console.log(this.fillGraphic);
            this.stage.children.forEach(shape => {
                shape.isFill = this.fillGraphic;
                shape.update();
            });
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
        this.stage.children.forEach(contour => {
            if (contour.type === ShapeTypeEnum.freepen) {
                if (this.actionType === ActionTypeEnum.shape && this.shapeType === ShapeTypeEnum.freepen2) {
                    contour.editable = true;
                } else {
                    contour.editable = false;
                }
            }
        });

        if (this.boundaryPts === undefined) {
            return;
        }
        if (!this.utils.isInPolygon(new Point(event.offsetX, event.offsetY), this.boundaryPts)) {
            console.log('out of image');
            return;
        }
        this.isMouseDown = true;

        if (this.actionType === ActionTypeEnum.nudge) {
            this.fader = this.getFader();
            this.fader.handleMouseDown(event);
            this.preFaderPos = this.fader.getCenter();
            this.nudgeHelper.setMode(this.fader.getCenter(), this.getFreepenCps(this.roi.id));
        }
        this.shape = this._createShape();
        if (this.shape != null) {
            this.shape.handleMouseDown(event);
        }
        // event.stopPropagation()不生效;查一下两个的区别是什么？
        // 只有在选择模式和自由笔2情况下才允许点击事件冒泡。
        if (this.actionType !== ActionTypeEnum.select && this.shapeType !== ShapeTypeEnum.freepen2) {
            event.stopImmediatePropagation();
        }
    }

    @HostListener('mousemove', ['$event']) onMouseMove(event: MouseEvent) {
        if (this.actionType === ActionTypeEnum.nudge) {
            this.fader = this.getFader();
            this.fader.handleMouseMove(event);
            if (this.fader.isMousedown) {
                const curFaderPos = this.fader.getCenter();
                const bridge = this.utils.creatBridgeRectCps(this.preFaderPos, curFaderPos, this.fader.getRadius());
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
            if (!this.shape.validate()) {
                this.stage.removeChild(this.shape);
            }
        }
        this.isMouseDown = false;

        if (this.actionType === ActionTypeEnum.nudge) {
            this.fader = this.getFader();
            this.fader.handleMouseUp(event);
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

    private _createShapeByContourType(shapeType: ContourTypeEnum) {
        let _shape = null;
        switch (shapeType) {
            case ContourTypeEnum.rectangle:
                _shape = new RectangleContainer(this.stage);
                break;
            case ContourTypeEnum.line:
                _shape = new LineContainer(this.stage);
                break;
            case ContourTypeEnum.freepen:
                _shape = new FreepenContainer(this.stage);
                break;
            case ContourTypeEnum.circle:
                _shape = new CircleContainer(this.stage);
                break;
            default:
                break;
        }
        return _shape;
    }

    private _createShape() {
        if (this.actionType !== ActionTypeEnum.shape) {
            return null;
        }
        let _shape = null;
        switch (this.shapeType) {
            case ShapeTypeEnum.line:
                _shape = new LineContainer(this.stage);
                break;
            case ShapeTypeEnum.rectangle:
                _shape = new RectangleContainer(this.stage);
                break;
            case ShapeTypeEnum.circle:
                _shape = new CircleContainer(this.stage);
                break;
            case ShapeTypeEnum.freepen:
                _shape = new FreepenContainer(this.stage);
                break;
            case ShapeTypeEnum.fader:
                _shape = new FaderContainer(this.stage);
                break;
            default:
                _shape = null;
        }
        if (_shape != null) {
            _shape.isFill = this.fillGraphic;
            _shape.setRoi(this.roi);
            _shape.setBoundaryPts(this.boundaryPts);
        }
        return _shape;
    }

    getFader() {
        if (this.fader == null || this.fader === undefined) {
            this.fader = new FaderContainer(this.stage);
            this.fader.isFill = this.fillGraphic;
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
            freepen.isFill = this.fillGraphic;
            freepen.setBoundaryPts(this.boundaryPts);
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

    update() {
        if (this.stage === undefined) {
            return;
        }

        this.stage.removeAllChildren();
        this.stage.clear();
        // this.updateBoundry();
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
            contours.push([graphic.roiuid, contour, graphic.type, graphic.contouruid]);
        });
        // draw graphics
        if (contours.length === 0) {
            return;
        }
        contours.forEach(contour => {
            const roiuid = contour[0];
            const cps = contour[1];
            cps.push(cps[0].copy());
            const loadShape = this._createShapeByContourType(contour[2]);
            // const freepen = new FreepenContainer(this.stage);
            loadShape.setRoi(this.rois.find(x => x.id === roiuid));
            loadShape.cps = cps;
            loadShape.contour_uid = contour[3];
            loadShape.isFill = this.fillGraphic;
            loadShape.setBoundaryPts(this.boundaryPts);
            loadShape.update();
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
