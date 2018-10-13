import { Component, ViewChild, OnInit, Input, Output, EventEmitter ,SimpleChanges, OnChanges,} from '@angular/core';
import { ConMessageService } from '../../shared/service/conMessage.service';
//import { PatientHttpService } from 'TpsShared/TpsService/patientHttp.service';
import { MenuItem } from '../../../patient-template/shared/patient-template.model'

@Component({
  selector: 'mpt-contouring-top-bar',
  templateUrl: './contouring-top-bar.component.html',
  styleUrls: ['./contouring-top-bar.component.less']
})
export class ContouringTopBarComponent implements OnInit {

  @Output() zoom: EventEmitter<any> = new EventEmitter<any>();
  @Output() wlww: EventEmitter<any> = new EventEmitter<any>();
  @Output() move: EventEmitter<any> = new EventEmitter<any>();
  @Output() sfile: EventEmitter<any> = new EventEmitter<any>();
  @Output() loadSeries: EventEmitter<any> = new EventEmitter<any>();
  @Output() auto: EventEmitter<any> = new EventEmitter<any>();
  @Output() remouse: EventEmitter<any> = new EventEmitter<any>();
  @Output() measure: EventEmitter<any> = new EventEmitter<any>();
  @Output() clear: EventEmitter<any> = new EventEmitter<any>();
  @Output() hideList: EventEmitter<any> = new EventEmitter<any>();
  @Input() seriesList:string[];

  constructor(
    private conMessage: ConMessageService,
    ) { 
    }
    seriesId: any;
    data: any[];
    atlasData: any = new Array();

    ngOnInit() {
      if (this.conMessage.serieses != undefined) {
          this.seriesId = this.conMessage.serieses;
      }
      this.conMessage.serieses$.subscribe(value => {
          this.seriesId = value;
      });
  }
  ngOnChanges(changes: SimpleChanges) {
       
  }

  display: boolean = false;
  addPlanDisplay: boolean = false;

  imgzoom() {
      this.zoom.emit();
  }

  imgmove() {
      this.move.emit();
  }

  imgwlww() {
      this.wlww.emit();
  }

  file() {
      this.sfile.emit();
  }

  removemose() {
      this.remouse.emit();
  }



  onItemClick(a) {
    //   if (a == 'Add ROI') {
    //       this.addPlanDisplay = true;
    //   }
    //   if (a == 'Atlas') {
    //       this.atlasData = [];
    //       this.patientHttp.autoSegmentation(0).subscribe((value: any) => {
    //           for (var i = 0; i < value.length; ++i) {
    //               let PTreeNode = {
    //                   label: value[i].autoSegmentationSite.siteName,
    //                   data: value[i].autoSegmentationSite.siteName,
    //                   expandedIcon: "fa fa-folder-open",
    //                   collapsedIcon: "fa fa-folder",
    //                   id: value[i].autoSegmentationSite.id,
    //                   children: []
    //               }
    //               this.atlasData.push(PTreeNode)
    //               // this.atlasData.push({'label': value[i].autoSegmentationSite.siteName});
    //               for (var j = 0; j < value[i].algorithmSites.items.length; j++) {
    //                   let childNode = {
    //                       label: value[i].algorithmSites.items[j].algorithmCodeName,
    //                       data: value[i].algorithmSites.items[j].algorithmCodeName,
    //                       icon: "fa fa-file-image-o",
    //                   }
    //                   this.atlasData[i].children.push(childNode);
    //               }
    //           }
    //           this.data = this.atlasData;
    //       });
    //       this.display = true;
    //   }
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
  OnMeasure(){
      this.conMessage.SetCurAction("measure");
  }

  OnDefaultMouseClick() {
      this.conMessage.SetCurAction("default");
  }
  OnBrushClick() {
      this.conMessage.SetCurAction("combo");
  }

  OnRectangleClick() {
      this.conMessage.SetCurAction("rectangle");
  }

  OnCircleClick(){
    this.conMessage.SetCurAction("circle1");
  }
  OnHideList(){
      this.hideList.emit();
  }
  OnSelect(){
    this.conMessage.SetCurAction("select");
  }


}

export class PTreeNode {
  public label: any;
  public data: any;
  public expandedIcon: any;
  public collapsedIcon: any;
  public children = new Array();
}
