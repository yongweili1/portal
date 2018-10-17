import { Point } from '../tools/point'
declare var createjs: any;

export class Hitbar extends createjs.Shape {
    constructor() {
        super()
        this.graphics.beginStroke("white").setStrokeStyle(10, "round")
    }

    set() { }
}