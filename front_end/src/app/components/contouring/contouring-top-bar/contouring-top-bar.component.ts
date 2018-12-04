import { Component, EventEmitter, Input, OnInit, Output, SimpleChanges, OnChanges } from '@angular/core';
import { MenuItem } from 'primeng/api';
import { EventAggregator } from '../../../shared/common/event_aggregator';
import { MacroService } from '../shared/service/macro.service';
import { ActionTypeEnum, ShapeTypeEnum } from '../../../shared/models/enums';


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
        EventAggregator.Instance().changeActionTypeEvent.publish(ActionTypeEnum.zoom);
    }

    onImgPan() {
        EventAggregator.Instance().changeActionTypeEvent.publish(ActionTypeEnum.pan);
    }
    onImgRotate() {
        EventAggregator.Instance().changeActionTypeEvent.publish(ActionTypeEnum.rotate);
    }
    onImgReset() {
        this.onReset.emit();
    }

    onImgWindow() {
        EventAggregator.Instance().changeActionTypeEvent.publish(ActionTypeEnum.window);
    }

    onImgCenter() {
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
        EventAggregator.Instance().changeActionTypeEvent.publish(ActionTypeEnum.select);
    }

    OnCrossSelect() {
        EventAggregator.Instance().changeActionTypeEvent.publish(ActionTypeEnum.locate);
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
        EventAggregator.Instance().changeActionTypeEvent.publish(ActionTypeEnum.shape);
        EventAggregator.Instance().changeShapeTypeEvent.publish(ShapeTypeEnum.line);
    }

    /**
    * 圆
    */
    FuncCircle() {
        EventAggregator.Instance().changeActionTypeEvent.publish(ActionTypeEnum.shape);
        EventAggregator.Instance().changeShapeTypeEvent.publish(ShapeTypeEnum.circle);
    }

    /**
    * 矩形
    */
    FuncRectangle() {
        EventAggregator.Instance().changeActionTypeEvent.publish(ActionTypeEnum.shape);
        EventAggregator.Instance().changeShapeTypeEvent.publish(ShapeTypeEnum.rectangle);
    }

    /**
    * 自由笔
    */
    FuncFreepen() {
        EventAggregator.Instance().changeActionTypeEvent.publish(ActionTypeEnum.shape);
        EventAggregator.Instance().changeShapeTypeEvent.publish(ShapeTypeEnum.freepen);
    }
    FuncFreepenModify() {
        EventAggregator.Instance().changeActionTypeEvent.publish(ActionTypeEnum.shape);
        EventAggregator.Instance().changeShapeTypeEvent.publish(ShapeTypeEnum.freepen2);
    }

    FuncNudge() {
        EventAggregator.Instance().changeActionTypeEvent.publish(ActionTypeEnum.nudge);
    }
}
