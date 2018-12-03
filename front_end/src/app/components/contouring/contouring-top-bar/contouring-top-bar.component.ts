import { Component, EventEmitter, Input, OnInit, Output, SimpleChanges, OnChanges } from '@angular/core';
import { MenuItem } from 'primeng/api';
import { EventAggregator } from '../../../shared/common/event_aggregator';
import { KeyValuePair } from '../../../shared/common/keyvaluepair';
import { MacroService } from '../shared/service/macro.service';

declare var actions: any;
declare var shapes: any;

@Component({
    selector: 'mpt-contouring-top-bar',
    templateUrl: './contouring-top-bar.component.html',
    styleUrls: ['./contouring-top-bar.component.less']
})
export class ContouringTopBarComponent implements OnInit, OnChanges {
    @Output() onReset: EventEmitter<any> = new EventEmitter<any>();
    @Output() onLoadVolume: EventEmitter<any> = new EventEmitter<any>();
    @Output() auto: EventEmitter<any> = new EventEmitter<any>();
    @Output() measure: EventEmitter<any> = new EventEmitter<any>();
    @Output() onClearGraphics: EventEmitter<any> = new EventEmitter<any>();
    @Output() onCenter: EventEmitter<any> = new EventEmitter<any>();
    @Output() onAddRoi: EventEmitter<any> = new EventEmitter<any>();
    @Output() onManageRoi: EventEmitter<any> = new EventEmitter<any>();
    @Output() autoroi: EventEmitter<any> = new EventEmitter<any>();

    @Input() seriesList: string[];

    constructor(
        private macroSvc: MacroService
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
    }

    onImgCenter() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.locate));
        this.onCenter.emit();
    }

    OnNewROI() {
        this.onAddRoi.emit();
    }

    OnManageROI() {
        this.onManageRoi.emit();
    }

    loadMPR() {
        this.onLoadVolume.emit(this.seriesId);
    }

    OnClearAllClick() {
        this.onClearGraphics.emit();
    }

    OnSelect() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.select));
    }

    OnCrossSelect() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.locate));
    }

    OnStartMacro() {
        this.macroSvc.macro('start').subscribe(response => {
            if (response.success) {
                console.log('start macro transcribe');
            } else {
                console.error(response.message);
            }
        });
    }
    OnEndMacro() {
        this.macroSvc.macro('finish').subscribe();
    }
    OnRunMacro() {
        this.macroSvc.macro('run').subscribe();
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
