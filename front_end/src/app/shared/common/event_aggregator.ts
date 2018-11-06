import { Subject } from 'rxjs/Subject';

export class EventAggregator {
    private static _aggregator:EventAggregator;

    public static Instance() {
        if (this._aggregator == null)
            this._aggregator = new EventAggregator()
        return this._aggregator;
    }

    //#region observable msg
    contourCps = new ObservableObject();
    //#endregion
    
    private constructor() { }
}

export class ObservableObject extends Subject<any> {
    constructor() {
        super()
        this.asObservable();
    }

    publish(data) {
        this.next(data)
    }
}