import { Component, OnInit, ViewChild, Injector, ElementRef } from '@angular/core';
import { Http, Response, Headers, RequestOptions } from '@angular/http';
import { ActivatedRoute, ParamMap } from '@angular/router';
import { HttpClient } from "@angular/common/http";
import { DialogModule } from 'primeng/dialog';

import { PatientDBService } from '../shared/patientDB.service';
//import { PatientCollection } from '../shared/PatientCollection';
import { SeriesHttpService } from '../shared/seriesHttp.service';
import { RoiHttpService } from '../shared/roiHttp.service';
import { StorageService } from '../shared/storage.service';

import {
  LazyLoadEvent, ConfirmationService, Paginator
} from 'primeng/primeng';

import { ToastService } from '../../../core/toast.service';
import { Page, PageRequest } from '../../../shared/models';


import { ContouringService } from '../shared/contouring.service';

declare var $: any;

@Component({
  selector: 'mpt-contouring',
  templateUrl: './contouring.component.html',
  styleUrls: ['./contouring.component.less']
})
export class ContouringComponent implements OnInit {
  patientID: any;
  imageX: any;
  imageY: any;
  imageZ: any;
  wl: any;
  ww: any;
  spacingX: any;
  spacingY: any;
  spacingZ: any;
  pixelRepresentation: any;
  rescaleSlope: any;
  rescaleIntercept: any;
  firstImagePosition: any;
  lastImagePosition: any;
  seriList: any;

  @ViewChild('picLeft1') picLeft1;
  @ViewChild('picLeft2') picLeft2;
  @ViewChild('picLeft3') picLeft3;
  @ViewChild('load') load;

  constructor(
    private injector: Injector, 
    private http: HttpClient, 
    public activeRoute: ActivatedRoute, 
    //private seriesHttp: SeriesHttpService, 
    private patientDB: PatientDBService,
    //private patientCollection: PatientCollection, 
    public roiHttp: RoiHttpService, 
    private storageService: StorageService, 
    private seriesHttpService: SeriesHttpService,
    ) {
      // this.activeRoute.queryParams.subscribe(params => {
      //     this.patientid = params['patientid'];
      // });
  }
  axialChange(event: any) {
      if (event[2] == 'ver') {
          this.picLeft3.getBuffer(event[0], 'sagittal', event[1])
          this.picLeft2.patient2screen(event[0]);
      }
      if (event[2] == 'cur') {
          this.picLeft2.getBuffer(event[0], 'coronal', event[1])
          this.picLeft3.patient2screen(event[0]);
      }
      if (event[2] == 'cro') {
          this.picLeft2.patient2screen(event[0]);
          this.picLeft3.patient2screen(event[0]);
          this.picLeft3.getBuffer(event[0], 'sagittal', event[1])
          this.picLeft2.getBuffer(event[0], 'coronal', event[1])
          // var a = new Array(Math.floor(this.imageX * event[0]), Math.floor(this.imageY * event[1]));
          // var b = new Array('sagittal', 'coronal');
          // this.getduffer(a, b, event[4])
      }
  }

  // getduffer(pageindex, tag, downsamplefactor) {
  //     let that = this;
  //     var data = { mprrequests: [{ sliceType: tag[0], sliceNumber: pageindex[0] }, { sliceType: tag[1], sliceNumber: pageindex[1] }], downsamplefactor: downsamplefactor };
  //     $.ajax({
  //         url: this.storageService.retrieve("PATIENT_API_URLS") + "services/app/Series/GetMprRawBuffer",
  //         type: "POST",
  //         dataType: 'JSON',
  //         // beforeSend: function(XMLHttpRequest) {
  //         //     XMLHttpRequest.setRequestHeader("Authorization", 'Bearer ' + abp.auth.getToken());
  //         // },
  //         async: false,
  //         contentType: 'application/json',
  //         data: JSON.stringify(data),
  //         success: function(res) {
  //             if (tag[1] == "coronal") {
  //                 var mprData = new Array(res.result.mprSliceDetails.Coronal.width, res.result.mprSliceDetails.Coronal.height, res.result.mprSliceDetails.Coronal.buffer)
  //                 that.picLeft2.getduffer(pageindex[1], tag[1], mprData);
  //             }
  //             if (tag[0] == "sagittal") {
  //                 var mprData = new Array(res.result.mprSliceDetails.Sagittal.width, res.result.mprSliceDetails.Sagittal.height, res.result.mprSliceDetails.Sagittal.buffer)
  //                 that.picLeft3.getduffer(pageindex[0], tag[0], mprData);
  //             }
  //         },
  //         error: function(er) {
  //             return;
  //         }
  //     });
  // }

  coronalChange(event: any) {
      if (event[2] == 'ver') {
          this.picLeft3.getBuffer(event[0], 'sagittal', event[1])
          this.picLeft1.patient2screen(event[0]);
      }
      if (event[2] == 'cur') {
          this.picLeft1.getBuffer(event[0], 'axial', event[1])
          this.picLeft3.patient2screen(event[0]);
      }
      if (event[2] == 'cro') {
          this.picLeft3.patient2screen(event[0]);
          this.picLeft1.patient2screen(event[0]);
          this.picLeft1.getBuffer(event[0], 'axial', event[1])
          this.picLeft3.getBuffer(event[0], 'sagittal', event[1])
      }
  }
  sagittalChange(event: any) {
      if (event[2] == 'ver') {
          this.picLeft2.getBuffer(event[0], 'coronal', event[1])
          this.picLeft1.patient2screen(event[0]);
      }
      if (event[2] == 'cur') {
          this.picLeft1.getBuffer(event[0], 'axial', event[1])
          this.picLeft2.patient2screen(event[0]);
      }
      if (event[2] == 'cro') {
          this.picLeft2.patient2screen(event[0]);
          this.picLeft1.patient2screen(event[0]);
          this.picLeft1.getBuffer(event[0], 'axial', event[1])
          this.picLeft2.getBuffer(event[0], 'coronal', event[1])
      }
  }

  aCross(event: any) {
      this.picLeft2.patient2screen(event);
      this.picLeft3.patient2screen(event);
  }
  bCross(event: any) {
      this.picLeft1.patient2screen(event);
      this.picLeft3.patient2screen(event);
  }
  cCross(event: any) {
      this.picLeft2.patient2screen(event);
      this.picLeft1.patient2screen(event);
  }

  ngOnInit() {
      if (this.patientDB.seriList != undefined) {
          this.seriList = this.patientDB.seriList[0];
      }
      this.patientDB.seriList$.subscribe(value => {
          this.seriList = value;
      });
      // $.contextMenu({
      //     selector: '#threebmp',
      //     callback: function (key, options) {
      //         var m = "clicked: " + key;
      //         console.log(m);
      //     },
      //     items: {
      //         "edit": { name: "选择/翻页", icon: "edit" },
      //         "cut": { name: "图像旋转", icon: "cut" },
      //         "status": {
      //             name: "测量",
      //             icon: "delete",
      //             items: {
      //                 "sub1": { name: "直线测距", icon: "edit" },
      //                 "sub2": { name: "像素", icon: "cut" },
      //             }
      //         },
      //         "normalSub": {
      //             name: "保存书签",
      //             items: {
      //                 "normalsub1": { 
      //                     name: "normal Sub 1",
      //                 },
      //                 "normalsub2": { 
      //                     name: "normal Sub 2",
      //                     items: {
      //                         "normalsub1": { name: "normal Sub 1"},
      //                         "normalsub2": { name: "normal Sub 2"},
      //                         "normalsub3": { name: "normal Sub 3"},
      //                     }
      //                 },
      //                 "normalsub3": { name: "normal Sub 3" },
      //             }
      //         }
      //     }
      // });
  }

  mainzoom() {
      this.picLeft1.zoom();
      this.picLeft2.zoom();
      this.picLeft3.zoom();
  }
  mainwlww() {
      this.picLeft1.changewl();
      this.picLeft2.changewl();
      this.picLeft3.changewl();
  }
  mainmove() {
      this.picLeft1.move();
      this.picLeft2.move();
      this.picLeft3.move();
  }
  sfile() {
      this.picLeft1.file();
  }
  remouse() {
      this.picLeft1.clearmouse();
      this.picLeft2.clearmouse();
      this.picLeft3.clearmouse();
  }
  test(){
      this.seriesHttpService.testService().subscribe((data)=>{
          alert(data);
      }

      )
  }
  
  sliceX: any; sliceY: any; sliceZ: any; gap: any; sliceAll: any;
  /**
   * 加载序列响应函数，暂时向service随便发一个seriesId
   * @param seriesId 
   */
  loadSeries() {
    let img = new Image();
    let seriesId:any = 'test1';
    this.seriesHttpService.GetSeriesPic(seriesId).subscribe((data) =>{
        img.src = data;
        img.onload = function(){
            let c= $(".a_class .icanvas").get(0);
            let ctx=c.getContext("2d");
    
            ctx.drawImage(img,20,20,1000,650);
    
            c= $(".b_class .icanvas").get(0);
            ctx=c.getContext("2d");
            ctx.drawImage(img,20,20,500,300);
    
            c= $(".c_class .icanvas").get(0);
            ctx=c.getContext("2d");
            ctx.drawImage(img,20,20,500,300);
       
        }
    },(error)=>{
        console.log(error);
    })

    // $.ajax({
    //     type: "GET",
    //     url: "http://10.9.19.139:8000/image/volumes?seriesuid=1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373",
    //  }).done(function (response, a, b, c) {
    //     img.src = response;
    //     img.onload = function(){
    //         let c= $(".a_class .icanvas").get(0);
    //         let ctx=c.getContext("2d");
        
    //         ctx.drawImage(img,20,20,1000,650);
        
    //         c= $(".b_class .icanvas").get(0);
    //         ctx=c.getContext("2d");
    //         ctx.drawImage(img,20,20,500,300);
        
    //         c= $(".c_class .icanvas").get(0);
    //         ctx=c.getContext("2d");
    //         ctx.drawImage(img,20,20,500,300);
       
    //     }
    //  }).fail(function (jqXHR, textStatus, errorThrown) {
    //     alert(jqXHR.responseJSON.error.message.value);
    //  });

    
        //   this.fb(12);
    //   this.seriesHttp.PostSeriesLoad(seriesId).subscribe(value => {
    //       this.imageX = this.seriList.rows;
    //       this.imageY = this.seriList.columns;
    //       this.imageZ = this.seriList.sliceCounts;
    //       this.wl = this.seriList.windowingLevel;
    //       this.ww = this.seriList.windowingWidth;
    //       this.spacingX = this.seriList.pixelSpacing[0];
    //       this.spacingY = this.seriList.pixelSpacing[1];
    //       this.pixelRepresentation = this.seriList.pixelRepresentation;
    //       this.rescaleSlope = this.seriList.rescaleSlope;
    //       this.rescaleIntercept = this.seriList.rescaleIntercept;
    //       this.firstImagePosition = this.seriList.firstImagePosition;
    //       this.lastImagePosition = this.seriList.lastImagePosition;
    //       this.sliceX = Math.floor(this.seriList.sliceCounts / 2);
    //       this.sliceY = Math.floor(this.seriList.columns / 2);
    //       this.sliceZ = Math.floor(this.seriList.rows / 2);
    //       if (this.firstImagePosition[2] < this.lastImagePosition[2]) {
    //           this.gap = new Array(this.spacingX, this.spacingY, this.seriList.pixelSpacing[2]);
    //       } else {
    //           this.gap = new Array(this.spacingX, this.spacingY, -this.seriList.pixelSpacing[2]);
    //       }
    //       this.sliceAll = new Array(this.sliceX, this.sliceY, this.sliceZ);
    //   });

  }
  mainmeasure(){
      
  }
  auto(node: any) {
      // var formData = { patientID: this.patientCollection.patient.Get(0).id, algorithmName: node[0], seriesID: this.patientCollection.patient.Get(0).series.Get(0).id };
      // this.roiHttp.PostCreateRoiByAtlas(formData).subscribe(value => {
      //     this.patientDB.SetRois(value.roiProperties);
      //     this.patientDB.Setcontour(value.roiGeometry.roiContourSets.items);
      //     this.picLeft1.GetContourSet();
      // });
  }

  fb(a) {
      this.load.loadbar(a)
  }

  message(w) {
      this.load.message(w);
  }
}
