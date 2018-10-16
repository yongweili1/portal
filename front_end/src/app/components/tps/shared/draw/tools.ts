import { Line } from '../overlay/line'
import { Rectangle } from '../overlay/rectangle'
import { Circle } from '../overlay/circle'
import { Ellipse } from '../overlay/ellipse'


export class Point {
    x: number;
    y: number;

    constructor(private _x: number, private _y: number) {
        this.x = _x;
        this.y = _y;
    }
}


export class SharpFactory {
    private static _factory: SharpFactory;

    private constructor() { 
    }

    static getInstance() {
        if (typeof(SharpFactory._factory) == 'undefined') {
            console.log('Create sharp factory');
            SharpFactory._factory = new SharpFactory();
        }
        return SharpFactory._factory;
    }

    public sharp(type: string, stage: any) {
        if (type == 'measure') {
            return new Line(stage);
        } else if (type == 'rectangle') {
            return new Rectangle(stage);
        } else if (type == 'circle') {
            return new Circle(stage);
        } else {

        }
    }
}
