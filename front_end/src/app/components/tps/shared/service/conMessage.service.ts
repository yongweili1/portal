import { Injectable } from '@angular/core';
import { Subject } from 'rxjs/Subject';
declare var THREE: any;
declare var MarchingSquaresJS: any;
declare var d3: any;
@Injectable()

export class ConMessageService {
    serieses: Array<any>;
    seriList: Array<any>;
    curAction: string;
    currentRois: Array<any>;
    contourset: Array<any>;

    private seriesesSource = new Subject<Array<any>>();//事件发射器 向观察者广播值
    private seriListSource = new Subject<Array<any>>();
    private curActionSource = new Subject<string>();
    private roisSource = new Subject<Array<any>>();
    private contourSource = new Subject<Array<any>>();

    serieses$ = this.seriesesSource.asObservable();// 作为可观察对象/订阅
    seriList$ = this.seriListSource.asObservable();
    curAction$ = this.curActionSource.asObservable();
    rois$ = this.roisSource.asObservable();
    contour$ = this.contourSource.asObservable();

    SetSerieses(serieses: Array<any>) {
        this.seriesesSource.next(serieses);//发布通知： 实际产生的数据，包括数字、字符串、对象等 选择tree的id
        this.serieses = serieses;
    }

    SetSeriList(seriList: Array<any>){
        this.seriListSource.next(seriList);//发布通知： 实际产生的数据，包括数字、字符串、对象等
        this.seriList = seriList;
    }

    SetCurAction(curAction: string) {
        this.curActionSource.next(curAction);
    }

    SetRois(rois: Array<any>) {
        this.roisSource.next(rois);
        this.currentRois = rois;
    }

    Setcontour(contour){
        this.contourSource.next(contour);
        this.contourset = contour;
    }
}
