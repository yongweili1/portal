import { FaderContainer } from '../../container/fader_container'
import { SharpFactory } from './sharp-factory'

export class FaderFactory implements SharpFactory{
    private static _factory: FaderFactory;
    
    private constructor() { 
    }

    static getInstance() {
        if (typeof(this._factory) == 'undefined') {
            console.log('Create nudge factory');
            this._factory = new FaderFactory();
        }
        return this._factory;
    }

    public createSharpContainer(stage: any) {
        return new FaderContainer(stage);
    }
}
