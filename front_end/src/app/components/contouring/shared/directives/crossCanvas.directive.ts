import { Directive, ElementRef, Input, OnChanges, OnInit, SimpleChanges } from '@angular/core';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
import { ExcuteHelper } from '../../../contouring/shared/tools/excute_helper';
import { Hitbar } from '../overlay/hitbar';
import { Point } from '../tools/point';
declare var createjs: any;

@Directive({
    selector: '[cross-canvas]'
})
export class CrossCanvasDirective implements OnInit, OnChanges {
    @Input() tag;
    @Input() point: Point;
    @Input() hColor: string;
    @Input() vColor: string;
    canvas: any;
    stage: any;
    excuteHelper: ExcuteHelper;
    horizontal: any;
    vertical: any;
    crossPoint: any;

    constructor(private el: ElementRef) { }

    ngOnInit() {
        console.log('[cross-canvas]ngOnInit');
        this.canvas = this.el.nativeElement;
        // init stage
        this.stage = new createjs.Stage(this.canvas);
        createjs.Touch.enable(this.stage);
        this.stage.enableMouseOver(50);
        this.stage.mouseMoveOutside = true;

        this.excuteHelper = new ExcuteHelper();

        // horizontal line
        this.horizontal = new createjs.Shape();
        this.horizontal.cursor = "url('/assets/img/vertical.cur'),auto";
        this.updateHorizontal();

        // vertical line
        this.vertical = new createjs.Shape();
        this.vertical.cursor = "url('/assets/img/horizontal.cur'),auto";
        this.updateVertical();

        // cross point
        this.crossPoint = new createjs.Shape();
        this.crossPoint.cursor = "url('/assets/img/move.cur'),auto";
        this.crossPoint.alpha = 0.2;
        this.updateCrossPoint();

        this.stage.addChild(this.horizontal, this.vertical, this.crossPoint);

        this.canvas.addEventListener('mouseup', this.handleMouseUp.bind(this))
        this.horizontal.addEventListener('pressmove', this.handlePressMove.bind(this));
        this.vertical.addEventListener('pressmove', this.handlePressMove.bind(this));
        this.crossPoint.addEventListener('pressmove', this.handlePressMove.bind(this));
        this.horizontal.addEventListener('pressup', this.handlePressUp.bind(this));
        this.vertical.addEventListener('pressup', this.handlePressUp.bind(this));
        this.crossPoint.addEventListener('pressup', this.handlePressUp.bind(this));
    }

    ngOnChanges(changes: SimpleChanges) {
        this.update();
    }

    private update() {
        if (this.point === undefined
            || this.vertical === undefined
            || this.horizontal === undefined
            || this.crossPoint === undefined) {
            return;
        }

        this.updateHorizontal();
        this.updateVertical();
        this.updateCrossPoint();
        this.stage.update();
    }

    private updateHorizontal() {
        this.horizontal.graphics.clear();
        this.horizontal.graphics.beginStroke(this.hColor)
            .setStrokeStyle(1, 'round').moveTo(0, this.point.y).lineTo(this.canvas.width, this.point.y);
        const horizontalHitbar = new Hitbar();
        horizontalHitbar.graphics.moveTo(0, this.point.y).lineTo(this.canvas.width, this.point.y);
        this.horizontal.hitArea = horizontalHitbar;
    }

    private updateVertical() {
        this.vertical.graphics.clear();
        this.vertical.graphics.beginStroke(this.vColor)
            .setStrokeStyle(1, 'round').moveTo(this.point.x, 0).lineTo(this.point.x, this.canvas.height);
        const verticalHitbar = new Hitbar();
        verticalHitbar.graphics.moveTo(this.point.x, 0).lineTo(this.point.x, this.canvas.height);
        this.vertical.hitArea = verticalHitbar;
    }

    private updateCrossPoint() {
        this.crossPoint.graphics.clear();
        this.crossPoint.graphics.beginFill('white').drawCircle(this.point.x, this.point.y, 8);
    }

    setCrossPoint(point: Point) {
        console.log(point.x, point.y);
        this.point = point;
    }

    handleMouseUp(evt) {
        this.setCrossPoint(new Point(evt.offsetX, evt.offsetY));
        this.update();

        EventAggregator.Instance().crossPoint.publish([this.tag, this.point]);
    }

    handlePressMove(evt) {
        if (evt.currentTarget === this.vertical) {
            this.setCrossPoint(new Point(evt.stageX, this.point.y));
        } else if (evt.currentTarget === this.horizontal) {
            this.setCrossPoint(new Point(this.point.x, evt.stageY));
        } else if (evt.currentTarget === this.crossPoint) {
            this.setCrossPoint(new Point(evt.stageX, evt.stageY));
        }
        this.update();
        console.log('move');
        if (!this.excuteHelper.canExcuteByCount()) {
            return;
        }
        console.log('excute');
        EventAggregator.Instance().crossPoint.publish([this.tag, this.point]);
    }

    handlePressUp(evt) {
        if (evt.currentTarget === this.vertical) {
            this.setCrossPoint(new Point(evt.stageX, this.point.y));
        } else if (evt.currentTarget === this.horizontal) {
            this.setCrossPoint(new Point(this.point.x, evt.stageY));
        } else if (evt.currentTarget === this.crossPoint) {
            this.setCrossPoint(new Point(evt.stageX, evt.stageY));
        }
        this.update();
        EventAggregator.Instance().crossPoint.publish([this.tag, this.point]);
    }
}
