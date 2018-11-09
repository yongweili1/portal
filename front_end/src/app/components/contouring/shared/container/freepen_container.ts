import { BaseContainer } from '../container/base_container'
import { Freepen } from '../overlay/freepen'
import { Point } from '../tools/point';
import { Text } from '../overlay/text';
import { EventAggregator } from '../../../../shared/common/event_aggregator';

export class FreepenContainer extends BaseContainer {
    shape: Freepen;
    text: Text;

    constructor(stage) {
        super(stage, 'freepen');
        this.shape = new Freepen(stage);
        this.initCps();
        this.text = new Text(stage, '* cps')
        this.addChild(this.shape, this.text);
    }

    update() {
        super.update()
        this.shape.update(this.cps)
        this.updateText()
    }

    setCps(cps) {
        this.cps = cps;
    }

    updateText() {
        this.text.setCp(this.getRightCp())
        this.text.setText('ControlPoint: ' + this.cps.length)
        this.text.update()
    }

    getRightCp() {
        let p = this.cps[0]
        this.cps.forEach(cp => {
            if (cp.x > p.x) {
                p = cp
            }
        });
        return p;
    }

    handleMouseDown(evt) {
        console.log('[freepen]handle MouseDown')
        super.handleMouseDown(evt)
        this.isMousedown = true;

        if (evt.target.type != 'freepen' 
            && evt.target.type != 'controlpoint' 
            && evt.target.type != 'text') {
            this.cps.push(new Point(evt.offsetX, evt.offsetY));
        }
    }
    handleMouseMove(evt) {
        if (this.isMousedown) {
            console.log('[freepen]handle MouseMove')
            this.isPaint = true;
            
            console.log('push a new point')
            this.cps.push(new Point(evt.offsetX, evt.offsetY));
            this.update();
        }
    }
    handleMouseUp(evt) {
        console.log('[freepen]handle MouseUp')
        if (this.isPaint) {
            console.log('[freepen]close the shape')
            this.cps.push(this.cps[0].copy());
            this.update();

            EventAggregator.Instance().contourCps.publish([this.uid, this.cps])
        }
        this.isMousedown = false;
        this.isPaint = false;
    }

    handlePressUp(evt) {
        super.handlePressUp(evt)
        this.handleMouseUp(evt)
    }

    handlePressMove(evt) {
        console.log('[freepen]handle PressMove')
        const delta_x = evt.stageX - this._tempPoint.x;
        const delta_y = evt.stageY - this._tempPoint.y;
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;

        if (evt.target == this.shape || evt.target == this.text) {
            this.cps.forEach(cp => {
                cp.offset(delta_x, delta_y);
            });
        }

        this.update();
    }
}