import { Component, ViewChild, OnInit, Input, Output, EventEmitter, SimpleChanges, OnChanges, } from '@angular/core';
import { ConMessageService } from '../shared/service/conMessage.service';
//import { PatientHttpService } from 'TpsShared/TpsService/patientHttp.service';
import { MenuItem } from 'primeng/api';
import { ContouringService } from '../shared/service/contouring.service'
import { KeyValuePair } from '../../../shared/common/keyvaluepair';
import { EventAggregator } from '../../../shared/common/event_aggregator';

declare var actions: any;
declare var shapes: any;

@Component({
    selector: 'mpt-contouring-top-bar',
    templateUrl: './contouring-top-bar.component.html',
    styleUrls: ['./contouring-top-bar.component.less']
})
export class ContouringTopBarComponent implements OnInit {

    @Output() zoom: EventEmitter<any> = new EventEmitter<any>();
    @Output() wlww: EventEmitter<any> = new EventEmitter<any>();
    @Output() pan: EventEmitter<any> = new EventEmitter<any>();
    @Output() rotate: EventEmitter<any> = new EventEmitter<any>();
    @Output() reset: EventEmitter<any> = new EventEmitter<any>();
    @Output() sfile: EventEmitter<any> = new EventEmitter<any>();
    @Output() loadSeries: EventEmitter<any> = new EventEmitter<any>();
    @Output() auto: EventEmitter<any> = new EventEmitter<any>();
    @Output() remouse: EventEmitter<any> = new EventEmitter<any>();
    @Output() measure: EventEmitter<any> = new EventEmitter<any>();
    @Output() clear: EventEmitter<any> = new EventEmitter<any>();
    @Output() hideList: EventEmitter<any> = new EventEmitter<any>();
    @Output() setcenter: EventEmitter<any> = new EventEmitter<any>();
    @Input() seriesList: string[];
    @Output() newroi: EventEmitter<any> = new EventEmitter<any>();
    @Output() manageroi: EventEmitter<any> = new EventEmitter<any>();
    @Output() autoroi: EventEmitter<any> = new EventEmitter<any>();

    constructor(
        private conMessage: ConMessageService,
        private conService: ContouringService
    ) {
    }
    seriesId: any;
    data: any[];
    atlasData: any = new Array();
    ROIItems:MenuItem[];

    ngOnInit() {
        this.ROIItems = [
            {label: 'Manage',icon: 'pi pi-refresh',command: () => {
                this.manageroi.emit();
            }},
        ];
    }
    ngOnChanges(changes: SimpleChanges) {

    }

    display: boolean = false;
    addPlanDisplay: boolean = false;

    onImgZoom() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.zoom));
        this.zoom.emit();
    }

    onImgPan() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.pan));
        this.pan.emit();
    }
    onImgRotate() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.rotate));
        this.rotate.emit();
    }
    onImgReset() {
        this.reset.emit();
    }

    onImgWindow() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.window));
        this.wlww.emit();
    }

    onImgCenter() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.locate));
        this.setcenter.emit();
    }

    file() {
        this.sfile.emit();
    }

    removemose() {
        this.remouse.emit();
    }


    onItemClick(a) {

    }

    OnNewROI(){
        this.newroi.emit();
    }
    
    OnManageROI(){
        this.manageroi.emit();
    }

    checkedNodes: any[] = null;
    dataA: any[] = [{
        "id": 1,
        "text": "My Documents",
        "children": [{
            "id": 11,
            "text": "Photos",
            "state": "closed",
            "children": [{
                "id": 111,
                "text": "Friend"
            }, {
                "id": 112,
                "text": "Wife"
            }, {
                "id": 113,
                "text": "Company"
            }]
        }, {
            "id": 12,
            "text": "Program Files",
            "children": [{
                "id": 121,
                "text": "Intel"
            }, {
                "id": 122,
                "text": "SKIN"
            }, {
                "id": 123,
                "text": "Microsoft Office"
            }, {
                "id": 124,
                "text": "Games"
            }]
        }, {
            "id": 13,
            "text": "index.html"
        }, {
            "id": 14,
            "text": "about.html"
        }, {
            "id": 15,
            "text": "welcome.html"
        }]
    }];

    //add roi
    private _roiName: any;
    private _roiDescription: any;
    private _roiCHNDescription: any;
    private _roiColor: any;
    _roiPlanningValue: any = 11;
    _roiBiologyValue: any = 11;

    planningRoleData: any[] = [
        { value: 11, text: "planRole1" },
        { value: 12, text: "planRole2" },
        { value: 13, text: "planRole3" }
    ]

    biologyRoleData: any[] = [
        { value: 11, text: "biologyRole1" },
        { value: 12, text: "biologyRole2" },
        { value: 13, text: "biologyRole3" }
    ]

    //自动分割
    hidelog(checkedNodes) {
        var nodes = new Array();
        for (let i = 0; i < checkedNodes.length; i++) {
            nodes[i] = checkedNodes[i].label;
        }
        this.display = false;
        this.auto.emit(nodes);
    }

    //getseries和MPR
    loadMPR() {
        this.loadSeries.emit(this.seriesId);
    }

    OnClearAllClick() {
        this.clear.emit();
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

    OnSegmentation(){
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

export class PTreeNode {
    public label: any;
    public data: any;
    public expandedIcon: any;
    public collapsedIcon: any;
    public children = new Array();
}
