import { Subject } from 'rxjs/Subject';

export class EventAggregator {
    private static _aggregator:EventAggregator;

    public static Instance() {
        if (this._aggregator == null)
            this._aggregator = new EventAggregator()
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
