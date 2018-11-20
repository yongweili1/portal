import { Directive, ElementRef, Input, OnInit } from '@angular/core';
import { Point } from '../tools/point';
import { Hitbar } from '../overlay/hitbar';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
import { LazyExcuteHandler } from '../../lazy_excute_handler';
declare var createjs: any;

@Directive({
    selector: '[cross-canvas]'
})
export class CrossCanvasDirective implements OnInit {
    @Input() name;
    @Input() point: Point;
    canvas: any;
    stage: any;
    context: CanvasRenderingContext2D;
    lazyExcuteHandler: LazyExcuteHandler;
    horizontal: any;
    vertical: any;
    crossPoint: any;
    horizontalColor: any;
    verticalColor: any;

    constructor(private el: ElementRef) { }

    ngOnInit() {
        console.log('[cross-canvas]ngOnInit');
        this.canvas = this.el.nativeElement;
        // init stage
        this.stage = new createjs.Stage(this.canvas);
        let width = this.canvas.width;
        let height = this.canvas.height;
        createjs.Touch.enable(this.stage);
        this.stage.enableMouseOver(50);
        this.stage.mouseMoveOutside = true;

        this.lazyExcuteHandler = new LazyExcuteHandler();

        // horizontal line
        this.horizontal = new createjs.Shape();
        if (this.name == "transverse") {
            this.horizontalColor = "#2196F3";
        } else if (this.name == "coronal") {
            this.horizontalColor = "#F44336";
        } else if (this.name == "saggital") {
            this.horizontalColor = "#F44336";
        }
        this.horizontal.graphics.beginStroke(this.horizontalColor)
            .setStrokeStyle(1, "round").moveTo(0, this.point.y).lineTo(this.canvas.width, this.point.y);
        let horizontalHitbar = new Hitbar();
        horizontalHitbar.graphics.moveTo(0, this.point.y).lineTo(this.canvas.width, this.point.y);
        this.horizontal.hitArea = horizontalHitbar;
        this.horizontal.cursor = "url('/assets/img/vertical.cur'),auto";

        // vertical line
        this.vertical = new createjs.Shape();
        if (this.name == "transverse") {
            this.verticalColor = "#CDDC39";
        } else if (this.name == "coronal") {
            this.verticalColor = "#CDDC39";
        } else if (this.name == "saggital") {
            this.verticalColor = "#2196F3";
        }
        this.vertical.graphics.beginStroke(this.verticalColor)
            .setStrokeStyle(1, "round").moveTo(this.point.x, 0).lineTo(this.point.x, this.canvas.height);
        var verticalHitbar = new Hitbar();
        verticalHitbar.graphics.moveTo(this.point.x, 0).lineTo(this.point.x, this.canvas.height);
        this.vertical.hitArea = verticalHitbar;
        this.vertical.cursor = "url('/assets/img/horizontal.cur'),auto";

        // cross point
        this.crossPoint = new createjs.Shape();
        this.crossPoint.graphics.beginFill("white").drawCircle(0, 0, 8);
        this.crossPoint.alpha = 0.2;
        this.crossPoint.cursor = "url('/assets/img/move.cur'),auto";

        this.stage.addChild(this.horizontal, this.vertical, this.crossPoint);

        this.canvas.addEventListener('mousedown', this.handleMouseDown.bind(this))
        this.horizontal.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.vertical.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.crossPoint.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.horizontal.addEventListener("pressup", this.handlePressUp.bind(this));
        this.vertical.addEventListener("pressup", this.handlePressUp.bind(this));
        this.crossPoint.addEventListener("pressup", this.handlePressUp.bind(this));
    }

    ngOnChanges() {
        this.update();
    }

    private update() {
        if (this.point === undefined
            || this.vertical === undefined
            || this.horizontal === undefined
            || this.crossPoint === undefined) {
            return;
        }

        this.horizontal.graphics.clear();
        this.vertical.graphics.clear();
        this.crossPoint.graphics.clear();
        this.horizontal.graphics.beginStroke(this.horizontalColor)
            .setStrokeStyle(1, "round").moveTo(0, this.point.y).lineTo(this.canvas.width, this.point.y);
        this.vertical.graphics.beginStroke(this.verticalColor)
            .setStrokeStyle(1, "round").moveTo(this.point.x, 0).lineTo(this.point.x, this.canvas.height);
        this.crossPoint.graphics.beginFill("white").drawCircle(this.point.x, this.point.y, 8);

        this.stage.update();
    }

    setCrossPoint(point: Point) {
        this.point = point;
    }

    handleMouseDown(evt) {
        if (evt.currentTarget == this.vertical
            || evt.currentTarget == this.horizontal
            || evt.currentTarget == this.crossPoint) return;
        if (evt.offsetX === undefined) {
            this.setCrossPoint(new Point(evt.stageX, evt.stageY));
        } else {
            this.setCrossPoint(new Point(evt.offsetX, evt.offsetY));
        }
        this.update();

        EventAggregator.Instance().crossPoint.publish([this.name, this.point]);
    }

    handlePressMove(evt) {
        if (evt.currentTarget == this.vertical) {
            this.setCrossPoint(new Point(evt.stageX, this.point.y));
        } else if (evt.currentTarget == this.horizontal) {
            evt.currentTarget.y = this.crossPoint.y = evt.stageY;
            this.setCrossPoint(new Point(this.point.x, evt.stageY));
        } else if (evt.currentTarget == this.crossPoint) {
            this.setCrossPoint(new Point(evt.stageX, evt.stageY));
        }
        this.update();
        console.log('move')
        if (!this.lazyExcuteHandler.canExcuteByCount()) return;
        console.log('excute')
        EventAggregator.Instance().crossPoint.publish([this.name, this.point]);
    }

    handlePressUp(evt) {
        if (evt.currentTarget == this.vertical) {
            this.setCrossPoint(new Point(evt.stageX, this.point.y));
        } else if (evt.currentTarget == this.horizontal) {
            evt.currentTarget.y = this.crossPoint.y = evt.stageY;
            this.setCrossPoint(new Point(this.point.x, evt.stageY));
        } else if (evt.currentTarget == this.crossPoint) {
            this.setCrossPoint(new Point(evt.stageX, evt.stageY));
        }
        this.update();
        EventAggregator.Instance().crossPoint.publish([this.name, this.point]);
    }
}
