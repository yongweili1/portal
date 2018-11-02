import { CrosslineContainer } from '../../container/crossline_container'
import { SharpFactory } from './sharp-factory'

export class CrosslineFactory implements SharpFactory{
    private static _factory: CrosslineFactory;

    private constructor() { 
    }

    static getInstance() {
        if (typeof(this._factory) == 'undefined') {
            console.log('Create crossline factory');
            this._factory = new CrosslineFactory();
        }
        return this._factory;
    }

    public createSharpContainer(stage: any) {
        return new CrosslineContainer(stage);
    }
}
