import { LineContainer } from '../../container/line_container'
import { SharpFactory } from './sharp-factory'

export class LineFactory implements SharpFactory{
    private static _factory: LineFactory;

    private constructor() { 
    }

    static getInstance() {
        if (typeof(this._factory) == 'undefined') {
            console.log('Create line factory');
            this._factory = new LineFactory();
        }
        return this._factory;
    }

    public createSharpContainer(stage: any) {
        return new LineContainer(stage);
    }
}
