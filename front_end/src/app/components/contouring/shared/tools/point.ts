export class Point {
    x: number;
    y: number;

    constructor(_x: number = 0, _y: number = 0) {
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

export class Point3d {
    x: number;
    y: number;
    z: number;

    constructor(_x: number = 0, _y: number = 0, _z: number = 0) {
        this.x = _x;
        this.y = _y;
        this.z = _z;
    }

    update(_x: number, _y: number, _z: number) {
        this.x = _x;
        this.y = _y;
        this.z = _z;
    }

    offset(delta_x: number, delta_y: number, delta_z: number) {
        if (delta_x != null) {
            this.x += delta_x;
        }
        if (delta_y != null) {
            this.y += delta_y;
        }
        if (delta_z != null) {
            this.z += delta_z;
        }
        return this;
    }

    copy() {
        return new Point3d(this.x, this.y, this.z);
    }

    equals(p: Point3d) {
        if (p.x === this.x && p.y === this.y && p.z === this.z) {
            return true;
        } else {
            return false;
        }
    }
}
