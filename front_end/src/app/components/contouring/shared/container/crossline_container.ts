import { BaseContainer } from '../container/base_container'
import { Point } from '../tools/point'
import { Hitbar } from '../overlay/hitbar';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
import { LazyExcuteHandler } from '../../lazy_excute_handler';
declare var createjs: any;
export class CrosslineContainer extends BaseContainer {
    center: Point;
    horizontal: any;
    vertical: any;
    crossPoint: any;
    tag: any;
    lazyExcuteHandler: LazyExcuteHandler;

    constructor(canvas, tag) {
        const stage = new createjs.Stage(canvas);
        super(stage, 'crossline');
        this.tag = tag;
        let width = this.stage.canvas.width;
        let height = this.stage.canvas.height;
        createjs.Touch.enable(this.stage);
        this.stage.enableMouseOver(50);
        this.stage.mouseMoveOutside = true;

        this.lazyExcuteHandler = new LazyExcuteHandler();

        this.horizontal = new createjs.Shape(); // 横线
        if (tag == "transverse") {
            this.horizontal.graphics.beginStroke("#2196F3").setStrokeStyle(1, "round").moveTo(0, 0).lineTo(width, 0);
        }
        if (tag == "coronal") {
            this.horizontal.graphics.beginStroke("#F44336").setStrokeStyle(1, "round").moveTo(0, 0).lineTo(width, 0);
        }
        if (tag == "saggital") {
            this.horizontal.graphics.beginStroke("#F44336").setStrokeStyle(1, "round").moveTo(0, 0).lineTo(width, 0);
        }
        let horizontalHitbar = new Hitbar()
        horizontalHitbar.graphics.moveTo(0, 0).lineTo(width, 0);
        this.horizontal.hitArea = horizontalHitbar
        this.horizontal.cursor = "url('/assets/img/vertical.cur'),auto";

        this.vertical = new createjs.Shape(); // 竖线
        if (tag == "transverse") {
            this.vertical.graphics.beginStroke("#CDDC39").setStrokeStyle(1, "round").moveTo(0, 0).lineTo(0, height);
        }
        if (tag == "coronal") {
            this.vertical.graphics.beginStroke("#CDDC39").setStrokeStyle(1, "round").moveTo(0, 0).lineTo(0, height);
        }
        if (tag == "saggital") {
            this.vertical.graphics.beginStroke("#2196F3").setStrokeStyle(1, "round").moveTo(0, 0).lineTo(0, height);
        }
        var verticalHitbar = new Hitbar();
        verticalHitbar.graphics.moveTo(0, 0).lineTo(0, height);
        this.vertical.hitArea = verticalHitbar;
        this.vertical.cursor = "url('/assets/img/horizontal.cur'),auto";

        this.crossPoint = new createjs.Shape(); // 交点
        this.crossPoint.graphics.beginFill("white").drawCircle(0, 0, 8);
        this.crossPoint.alpha = 0.2;
        this.crossPoint.cursor = "url('/assets/img/move.cur'),auto";

        this.addChild(this.horizontal, this.vertical, this.crossPoint)

        stage.canvas.addEventListener('mousedown', this.handleMouseDown.bind(this))
        this.horizontal.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.vertical.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.crossPoint.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.horizontal.addEventListener("pressup", this.handlePressUp.bind(this));
        this.vertical.addEventListener("pressup", this.handlePressUp.bind(this));
        this.crossPoint.addEventListener("pressup", this.handlePressUp.bind(this));

        this.setCenter(width / 2, height / 2);
        this.update();
    }

    update() {
        super.update()
        this.vertical.x = this.center.x;
        this.horizontal.y = this.center.y;
        this.crossPoint.x = this.vertical.x;
        this.crossPoint.y = this.horizontal.y;
        this.overlayStage.update();
    }

    setCenter(x, y) {
        this.center = new Point(x, y);
    }

    handleMouseDown(evt) {
        if (evt.currentTarget == this.vertical
            || evt.currentTarget == this.horizontal
            || evt.currentTarget == this.crossPoint) return;
        if (evt.offsetX === undefined) {
            this.setCenter(evt.stageX, evt.stageY);
        } else {
            this.setCenter(evt.offsetX, evt.offsetY);
        }
        this.update();

        EventAggregator.Instance().crossPoint.publish([this.tag, this.center]);
    }

    handlePressMove(evt) {
        if (evt.currentTarget == this.vertical) {
            this.setCenter(evt.stageX, this.center.y);
        } else if (evt.currentTarget == this.horizontal) {
            evt.currentTarget.y = this.crossPoint.y = evt.stageY;
            this.setCenter(this.center.x, evt.stageY);
        } else if (evt.currentTarget == this.crossPoint) {
            this.setCenter(evt.stageX, evt.stageY);
        }
        this.update();
        console.log('move')
        if (!this.lazyExcuteHandler.canExcuteByCount()) return;
        console.log('excute')
        EventAggregator.Instance().crossPoint.publish([this.tag, this.center]);
    }

    handlePressUp(evt) {
        if (evt.currentTarget == this.vertical) {
            this.setCenter(evt.stageX, this.center.y);
        } else if (evt.currentTarget == this.horizontal) {
            evt.currentTarget.y = this.crossPoint.y = evt.stageY;
            this.setCenter(this.center.x, evt.stageY);
        } else if (evt.currentTarget == this.crossPoint) {
            this.setCenter(evt.stageX, evt.stageY);
        }
        this.update();
        EventAggregator.Instance().crossPoint.publish([this.tag, this.center]);
    }
}
