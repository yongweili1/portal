import { Point } from '../tools/point';
import { ShapeTypeEnum } from '../../../../shared/models/enums';

declare var createjs: any;

export class Text extends createjs.Text {
    type = ShapeTypeEnum.text;
    overlayStage: any;
    protected _cp: Point;
    protected _is_hover: boolean;
    protected _color: string;
    text: string;

    constructor(stage, text) {
        super(text, '14px Arial', 'yellow');
        this.overlayStage = stage;
        this._is_hover = false;
        this.color = 'yellow';

        this.addEventListener('mouseover', this.handleMouseOver.bind(this));
        this.addEventListener('mouseout', this.handleMouseOut.bind(this));
        this.overlayStage.addChild(this);
    }

    setText(text) {
        this.text = text;
    }

    setCp(point) {
        this._cp = point;
    }

    update() {
        this.overlayStage.clear();
        if (this._cp == null) {
            return;
        }
        this.x = this._cp.x + 10;
        this.y = this._cp.y;
        this.overlayStage.update();
    }

    handleMouseOver(evt) {
        this._is_hover = true;
        this.color = 'red';
        this.update();
    }
    handleMouseOut(evt) {
        this._is_hover = false;
        this.color = 'yellow';
        this.update();
    }
}
