import { RectangleContainer } from '../../container/rectangle_container'
import { SharpFactory } from './sharp-factory'

export class RectangleFactory implements SharpFactory{
    private static _factory: RectangleFactory;

    private constructor() { 
    }

    static getInstance() {
        if (typeof(this._factory) == 'undefined') {
            console.log('Create rectangle factory');
            this._factory = new RectangleFactory();
        }
        return this._factory;
    }

    public createSharpContainer(stage: any) {
        return new RectangleContainer(stage);
    }
}
