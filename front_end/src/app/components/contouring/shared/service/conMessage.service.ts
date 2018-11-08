import { Injectable } from '@angular/core';
import { Subject } from 'rxjs/Subject';
import { KeyValuePair } from '../../../../shared/common/keyvaluepair';
import { ROIConfig } from '../model/ROIConfig.model'
declare var THREE: any;
declare var MarchingSquaresJS: any;
declare var d3: any;
@Injectable()

export class ConMessageService {
    serieses: Array<any>;
    seriList: Array<any>;
    currentRois: Array<any>;
    contourset: Array<any>;
    actionInfo: KeyValuePair;

    private seriesesSource = new Subject<Array<any>>();//事件发射器 向观察者广播值
    private seriListSource = new Subject<Array<any>>();
    private roisSource = new Subject<Array<any>>();
    private activeRoiSource = new Subject<ROIConfig>();
    private contourSource = new Subject<Array<any>>();
    private graphicsSource = new Subject<Array<any>>();
    private actionInfoSource = new Subject<KeyValuePair>();

    serieses$ = this.seriesesSource.asObservable();// 作为可观察对象/订阅
    seriList$ = this.seriListSource.asObservable();
    rois$ = this.roisSource.asObservable();
    activeRoi$ = this.activeRoiSource.asObservable();
    contour$ = this.contourSource.asObservable();
    graphics$ = this.graphicsSource.asObservable();
    actionInfo$ = this.actionInfoSource.asObservable();

    SetSerieses(serieses: Array<any>) {
        this.seriesesSource.next(serieses);//发布通知： 实际产生的数据，包括数字、字符串、对象等 选择tree的id
        this.serieses = serieses;
    }

    SetSeriList(seriList: Array<any>){
        this.seriListSource.next(seriList);//发布通知： 实际产生的数据，包括数字、字符串、对象等
        this.seriList = seriList;
    }

    SetRois(rois: Array<any>) {
        this.roisSource.next(rois);
        this.currentRois = rois;
    }

    SetActiveRoi(acriveRoi: ROIConfig) {
        this.activeRoiSource.next(acriveRoi);
    }

    Setcontour(contour){
        this.contourSource.next(contour);
        this.contourset = contour;
    }

    setGraphics(graphics) {
        this.graphicsSource.next(graphics);
    }

    setActionInfo(info) {
        this.actionInfoSource.next(info);
    }
}
