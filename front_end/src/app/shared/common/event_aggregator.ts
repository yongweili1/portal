import { Subject } from 'rxjs/Subject';

export class EventAggregator {
    private static _aggregator: EventAggregator;

    //#region observable msg
    selectRoiEvent = new ObservableObject();
    saveContoursEvent = new ObservableObject();
    deleteContoursEvent = new ObservableObject();
    saveSigleContourEvent = new ObservableObject();
    updateSigleContourEvent = new ObservableObject();
    deleteSigleContourEvent = new ObservableObject();
    contourReadyEvent = new ObservableObject();
    changeShapeTypeEvent = new ObservableObject();
    changeActionTypeEvent = new ObservableObject();
    faderRadiusDelta = new ObservableObject();
    volumnSize = new ObservableObject();
    sliceIndex = new ObservableObject();
    pageDelta = new ObservableObject();
    crossPoint = new ObservableObject();
    eventData = new ObservableObject();
    clearGraphicsEvent = new ObservableObject();
    //#endregion

    public static Instance() {
        if (this._aggregator == null) {
            this._aggregator = new EventAggregator();
        }
        return this._aggregator;
    }

    private constructor() { }
}

export class ObservableObject extends Subject<any> {
    constructor() {
        super();
        this.asObservable();
    }

    publish(data) {
        this.next(data);
    }
}
