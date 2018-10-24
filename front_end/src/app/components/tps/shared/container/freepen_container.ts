import { BaseContainer } from '../container/base_container'
import { Freepen } from '../overlay/freepen'
import { ControlPoint } from '../overlay/controlpoint'
import { Point } from '../tools/point';
import { Text } from '../overlay/text';

export class FreepenContainer extends BaseContainer {
    shape: any;
    cps: Array<ControlPoint>;
    text: Text;
    editable: false;
    nearestStartCp: any;
    nearestEndCp: any;

    extendShape: any;
    extendCps: Array<ControlPoint>;

    constructor(stage) {
        super(stage, 'freepen');
        this.shape = new Freepen(stage);
        this.cps = new Array();
        this.extendShape = new Freepen(stage);
        this.extendCps =new Array();
        this.text = new Text(stage, '* cps')
        this.addChild(this.shape, this.text);
        this.status = '';
    }

    update() {
        super.update()
        this.shape.update()
        // this.cps.forEach(cp => {
        //     cp.update()
        // });
        this.updateText()
        this.extendShape.update()
    }

    createCp(point) {
        const cp = new ControlPoint(this.overlayStage)
        cp.setCenter(point)
        this.addChild(cp)
        this.cps.push(cp)
    }

    updateCps(delta_x, delta_y) {
        this.cps.forEach(cp => {
            cp.updateCenter(delta_x, delta_y)
        });
        this.shape.moveCps(delta_x, delta_y);
    }
    
    createExtendCp(point) {
        const cp = new ControlPoint(this.overlayStage)
        cp.setCenter(point)
        this.extendCps.push(cp)
    }
    
    clearPaint() {
        this.extendCps = new Array()
        this.extendShape.updateCps(this.extendCps)
    }

    getNearestCp(p: Point) {
        let index = 0;
        let nearestCp = this.cps[0];
        let distance = this.getDistance(p, nearestCp.getCenter())
        for (let i = 0; i < this.cps.length; i++) {
            const cp = this.cps[i];
            let point = cp.getCenter()
            let dis = Math.sqrt((point.x - p.x)**2 + (point.y - p.y)**2)
            if (dis < distance) {
                index = i;
                distance = dis;
                nearestCp = cp;
            }
        }
        
        return [index, nearestCp];
    }

    getLength(start, end, points) {
        let length = 0
        for (let index = start + 1; index < end; index++) {
            length += this.getDistance(points[index - 1].getCenter(), points[index].getCenter())
        }
        return length
    }

    getDistance(p1:Point, p2:Point) {
        return Math.sqrt((p1.x - p2.x)**2 + (p1.y - p2.y)**2);
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
    compareCp(cp1, cp2) {
        let p1 = cp1.getCenter()
        let p2 = cp2.getCenter()
        if (p1.x == p2.x && p1.y == p2.y) {
            return true;
        } else {
            return false;
        }
    }
    modifyShape() {
        let range = [this.nearestStartCp[0], this.nearestEndCp[0]]
        if (this.nearestStartCp[0] > this.nearestEndCp[0]) {
            range = [this.nearestEndCp[0], this.nearestStartCp[0]]
            // this.extendCps.reverse()
        }
        let totalDistance = this.getLength(0, this.cps.length, this.cps)
        let distance = this.getLength(range[0], range[1], this.cps)
        let _cps = new Array()
        if (totalDistance - distance > distance) {
            console.log('A')
            for (let index = 0; index < range[0]; index++) {
                _cps.push(this.cps[index])
            }
            if (!this.compareCp(this.extendCps[0], this.cps[range[0]])) {
                this.extendCps.reverse()
            }
            this.extendCps.splice(0, 1)
            this.extendCps.pop()
            _cps.push.apply(_cps, this.extendCps)
            for (let index = range[1] + 1; index < this.cps.length; index++) {
                _cps.push(this.cps[index])
            }
        } else {
            console.log('B')
            for (let index = range[0] + 1; index < range[1]; index++) {
                const cp = this.cps[index];
                _cps.push(cp)
            }
            if (!this.compareCp(this.extendCps[0], this.cps[range[1]])) {
                this.extendCps.reverse()
            }
            this.extendCps.splice(0, 1)
            this.extendCps.pop()
            _cps.push.apply(_cps, this.extendCps)
        }
        for (let index = 0; index < this.cps.length; index++) {
            this.removeChildAt(2)
        }
        _cps.push(_cps[0])
        this.cps = _cps;
        this.shape.updateCps(_cps);
        this.cps.forEach(cp => {
            this.addChild(cp)
        });
        this.clearPaint()
    }

    handleMouseDown(evt) {
        console.log('[freepen]handle MouseDown')
        super.handleMouseDown(evt)
        this.isMousedown = true;

        if (this.editable) {
            const point = new Point(evt.stageX, evt.stageY)
            this.nearestStartCp = this.getNearestCp(point)
            this.extendShape.appendCp(this.nearestStartCp[1].getCenter())
            this.createExtendCp(this.nearestStartCp[1].getCenter())
            this.extendShape.appendCp(point)
            this.createExtendCp(point)
        } else {    
            if (evt.target.type != 'freepen' 
                && evt.target.type != 'controlpoint' 
                && evt.target.type != 'text') {
                const point = new Point(evt.offsetX, evt.offsetY)
                this.shape.appendCp(point);
                this.createCp(point)
            }
        }
    }
    handleMouseMove(evt) {
        if (this.isMousedown) {
            console.log('[freepen]handle MouseMove')
            this.isPaint = true;
            
            if (this.editable) {
                const point = new Point(evt.stageX, evt.stageY)
                this.extendShape.appendCp(point)
                this.createExtendCp(point)
                this.update();
            } else {
                const point = new Point(evt.offsetX, evt.offsetY)
                this.shape.appendCp(point)
                this.createCp(point)
                this.update();
            }
        }
    }
    handleMouseUp(evt) {
        console.log('[freepen]handle MouseUp')
        if (this.isPaint) {
            if (this.editable) {
                const point = new Point(evt.stageX, evt.stageY)
                this.extendShape.appendCp(point)
                this.createExtendCp(point)

                this.nearestEndCp = this.getNearestCp(point)
                this.extendShape.appendCp(this.nearestEndCp[1].getCenter())
                this.createExtendCp(this.nearestEndCp[1].getCenter())
                
                this.modifyShape()
                this.update();
            } else {
                console.log('[freepen]close the shape')
                this.shape.appendCp(this.shape.getCp(0))
                this.update();
            }
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

        if (this.editable) {
            this.handleMouseMove(evt);
            return;
        }
        if (evt.target == this.shape || evt.target == this.text) {
            this.x += delta_x;
            this.y += delta_y;
            // this.updateCps(delta_x, delta_y);
        } else {
            evt.target.updateCenter(delta_x, delta_y);
        }

        this.update();
    }
}