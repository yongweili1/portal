export class Point {
    x: number;
    y: number;

    constructor(_x: number, _y: number) {
        this.x = _x;
        this.y = _y;
    }

    update(_x: number, _y: number) {
        this.x = _x;
        this.y = _y;
    }

    offset(delta_x: number, delta_y: number) {
        if (delta_x != null) {
            this.x += delta_x;
        }
        if (delta_y != null) {
            this.y += delta_y;
        }
        return this;
    }

    copy() {
        return new Point(this.x, this.y);
    }

    equals(p: Point) {
        if (p.x === this.x && p.y === this.y) {
            return true;
        } else {
            return false;
        }
    }
}
