import { Subject } from 'rxjs/Subject';

export class MsgAggregator {
    private static _aggregator:MsgAggregator;

    public static Instance() {
        if (this._aggregator == null)
            this._aggregator = new MsgAggregator()
        return this._aggregator;
    }

    //#region Msg
    private contourCpsSource = new Subject<Array<any>>();
    //#endregion

    //#region observable msg
    contourCps$ = this.contourCpsSource.asObservable();
    //#endregion
    
    private constructor() { }

    //#region publish msg
    publishContourCps(cps) {
        this.contourCpsSource.next(cps);
    }

    //#endregion
}
