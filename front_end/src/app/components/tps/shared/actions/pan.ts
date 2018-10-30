import { BaseAction } from "./base";
import { Point } from "../tools/point";

export class Pan extends BaseAction {
    prePos: any;

    constructor() {
        super()
        this.prePos = [0, 0]
    }

    handleMouseDown(evt) {
        this.prePos = [evt.clientX, evt.clientY];
    }

    handleMouseMove(evt) {
        // if (!this.lazyExcuteHandler.canExcuteByCount()) return;
        // let curPos = [evt.clientX, evt.clientY];
        // that.panReq.emit([this.tag, this.prePos, curPos]);
        // this.prePos = curPos;
    }

    handleMouseUp(evt) {
        
    }
}