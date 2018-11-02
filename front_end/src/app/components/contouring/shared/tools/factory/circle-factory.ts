import { CircleContainer } from '../../container/circle_container'
import { SharpFactory } from './sharp-factory'

export class CircleFactory implements SharpFactory{
    private static _factory: CircleFactory;

    private constructor() { 
    }

    static getInstance() {
        if (typeof(this._factory) == 'undefined') {
            console.log('Create circle factory');
            this._factory = new CircleFactory();
        }
        return this._factory;
    }

    public createSharpContainer(stage: any) {
        return new CircleContainer(stage);
    }
}
