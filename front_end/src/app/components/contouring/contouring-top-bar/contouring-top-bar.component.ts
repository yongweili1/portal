import { Component, EventEmitter, Input, OnInit, Output, SimpleChanges } from '@angular/core';
import { MenuItem } from 'primeng/api';
import { EventAggregator } from '../../../shared/common/event_aggregator';
import { KeyValuePair } from '../../../shared/common/keyvaluepair';
import { ContouringService } from '../shared/service/contouring.service';

declare var actions: any;
declare var shapes: any;

@Component({
    selector: 'mpt-contouring-top-bar',
    templateUrl: './contouring-top-bar.component.html',
    styleUrls: ['./contouring-top-bar.component.less']
})
export class ContouringTopBarComponent implements OnInit {
    @Output() onReset: EventEmitter<any> = new EventEmitter<any>();
    @Output() loadSeries: EventEmitter<any> = new EventEmitter<any>();
    @Output() auto: EventEmitter<any> = new EventEmitter<any>();
    @Output() measure: EventEmitter<any> = new EventEmitter<any>();
    @Output() onClearGraphics: EventEmitter<any> = new EventEmitter<any>();
    @Output() hideList: EventEmitter<any> = new EventEmitter<any>();
    @Output() setcenter: EventEmitter<any> = new EventEmitter<any>();
    @Input() seriesList: string[];
    @Output() onAddRoi: EventEmitter<any> = new EventEmitter<any>();
    @Output() onManageRoi: EventEmitter<any> = new EventEmitter<any>();
    @Output() autoroi: EventEmitter<any> = new EventEmitter<any>();

    constructor(
        private conService: ContouringService
    ) {
    }
    seriesId: any;
    data: any[];
    atlasData: any = new Array();
    ROIItems: MenuItem[];

    ngOnInit() {
        this.ROIItems = [
            {
                label: 'Manage', icon: 'pi pi-refresh', command: () => {
                    this.onManageRoi.emit();
                }
            },
        ];
    }

    ngOnChanges(changes: SimpleChanges) {

    }

    display: boolean = false;
    addPlanDisplay: boolean = false;

    onImgZoom() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.zoom));
    }

    onImgPan() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.pan));
    }
    onImgRotate() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.rotate));
    }
    onImgReset() {
        this.onReset.emit();
    }

    onImgWindow() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.window));
        // this.wlww.emit();
    }

    onImgCenter() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.locate));
        this.setcenter.emit();
    }

    OnNewROI() {
        this.onAddRoi.emit();
    }

    OnManageROI() {
        this.onManageRoi.emit();
    }

    loadMPR() {
        this.loadSeries.emit(this.seriesId);
    }

    OnClearAllClick() {
        this.onClearGraphics.emit();
    }

    OnHideList() {
        this.hideList.emit();
    }

    OnSelect() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.select));
    }

    OnCrossSelect() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.locate));
    }

    OnStartMacro() {
        this.conService.Macro("start").subscribe(result => {
            if (result == "ok") {
                console.log("start macro transcribe");
            }
        }, (error) => {
            console.log("start macro transcribe failed")
        });
    }
    OnEndMacro() {
        this.conService.Macro("finish").subscribe();
    }
    OnRunMacro() {
        this.conService.Macro("run").subscribe();
    }

    OnSegmentation() {
        this.autoroi.emit();
    }

    /**
    * 测量，直线
    */
    FuncMeasure() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.shape, shapes.line));
    }

    /**
    * 圆
    */
    FuncCircle() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.shape, shapes.circle));
    }

    /**
    * 矩形
    */
    FuncRectangle() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.shape, shapes.rectangle));
    }

    /**
    * 自由笔
    */
    FuncFreepen() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.shape, shapes.freepen));
    }
    FuncFreepenModify() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.shape, shapes.freepen_edit));
    }

    FuncNudge() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.nudge));
    }
}
