import { BaseContainer } from '../container/base_container'
import { Freepen } from '../overlay/freepen'
import { ControlPoint } from '../overlay/controlpoint'
import { Point } from '../tools/point';
import { Text } from '../overlay/text';

export class FreepenContainer extends BaseContainer {
    shape: any;
    cps: Array<ControlPoint>;
    text: Text;

    constructor(stage) {
        super(stage, 'freepen');
        this.shape = new Freepen(stage);
        this.cps = new Array();
        this.text = new Text(stage, '* cps')
        this.addChild(this.shape, this.text);
    }

    update() {
        super.update()
        this.shape.update()
        this.cps.forEach(cp => {
            cp.update()
        });
        this.updateText()
    }

    createCp(point) {
        const cp = new ControlPoint(this.overlayStage)
        cp.setCenter(point)
        this.addChild(cp)
        this.cps.push(cp)
    }

    updateText() {
        this.text.setCp(this.getRightCp())
        this.text.setText('ControlPoint: ' + this.cps.length)
        this.text.update()
    }

    getRightCp() {
        let p = this.cps[0].getCenter()
        this.cps.forEach(cp => {
            if (cp.getCenter().x > p.x) {
                p = cp.getCenter()
            }
        });
        return p;
    }

    handleMouseDown(evt) {
        console.log('[freepen]handle MouseDown')
        super.handleMouseDown(evt)
        this.isMousedown = true;
        if (evt.target.type != 'freepen' && evt.target.type != 'controlpoint' && evt.target.type != 'text') {
            const point = new Point(evt.offsetX, evt.offsetY)
            this.shape.appendCp(point);
            this.createCp(point)
        }
    }
    handleMouseMove(evt) {
        if (this.isMousedown) {
            console.log('[freepen]handle MouseMove')
            this.isPaint = true;
            const point = new Point(evt.offsetX, evt.offsetY)
            this.shape.appendCp(point)
            this.createCp(point)
            this.update();
        }
    }
    handleMouseUp(evt) {
        console.log('[freepen]handle MouseUp')
        if (this.isPaint) {
            console.log('[freepen]close the shape')
            this.shape.appendCp(this.shape.getCp(0))
            this.update();
        }
        this.isMousedown = false;
        this.isPaint = false;
    }

    handlePressMove(evt) {
        console.log('[freepen]handle PressMove')
        const delta_x = evt.stageX - this._tempPoint.x;
        const delta_y = evt.stageY - this._tempPoint.y;
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;

        if (evt.target == this.shape || evt.target == this.text) {
            this.x += delta_x;
            this.y += delta_y;
        } else {
            evt.target.updateCenter(delta_x, delta_y)
        }

        this.update();
    }
}