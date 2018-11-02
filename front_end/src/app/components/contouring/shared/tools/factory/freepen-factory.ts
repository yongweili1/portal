import { FreepenContainer } from '../../container/freepen_container'
import { SharpFactory } from './sharp-factory'

export class FreepenFactory implements SharpFactory{
    private static _factory: FreepenFactory;

    private constructor() { 
    }

    static getInstance() {
        if (typeof(this._factory) == 'undefined') {
            console.log('Create crossline factory');
            this._factory = new FreepenFactory();
        }
        return this._factory;
    }

    public createSharpContainer(stage: any) {
        return new FreepenContainer(stage);
    }
}
