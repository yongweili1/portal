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
    }

    handleMouseUp(evt) {
    }
}
