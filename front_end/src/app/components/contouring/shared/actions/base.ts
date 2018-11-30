export class BaseAction {
    tag: string;
    isMouseDown = false;

    constructor (tag) {
        this.tag = tag;
    }

    handleMouseDown(e) {
        this.isMouseDown = true;
    }

    handleMouseMove(e) {}

    handleMouseUp(e) {
        this.isMouseDown = false;
    }
}
