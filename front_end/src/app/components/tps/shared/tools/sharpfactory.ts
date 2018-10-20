import { LineContainer } from '../container/line_container'
import { CircleContainer } from '../container/circle_container'
import { RectangleContainer } from '../container/rectangle_container'

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
            return new LineContainer(stage);
        } else if (type == 'rectangle') {
            return new RectangleContainer(stage);
        } else if (type == 'circle') {
            return new CircleContainer(stage);
        } else {

        }
    }
}
