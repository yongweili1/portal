import { Component, OnInit, ViewChild, Injector, ElementRef } from '@angular/core';
import { ActivatedRoute, ParamMap } from '@angular/router';
import { HttpClient } from "@angular/common/http";
import { DialogModule } from 'primeng/dialog';

import { ConMessageService } from '../shared/service/conMessage.service';
//import { PatientCollection } from '../shared/PatientCollection';
import { SeriesHttpService } from '../shared/service/seriesHttp.service';
import { RoiHttpService } from '../shared/service/roiHttp.service';
import { StorageService } from '../shared/service/storage.service';
import {SlimLoadingBarService} from 'ng2-slim-loading-bar';


import {
  LazyLoadEvent, ConfirmationService, Paginator
} from 'primeng/primeng';

import { ToastService } from '../../../core/toast.service';
import { Page, PageRequest } from '../../../shared/models';


import { ContouringService } from '../shared/service/contouring.service';
import { Observable } from 'rxjs/Observable';

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
  action:any;
  display: boolean = false;
  
  @ViewChild('picLeft1') picLeft1;
  @ViewChild('picLeft2') picLeft2;
  @ViewChild('picLeft3') picLeft3;
  @ViewChild('load') load;

  constructor(
    private injector: Injector, 
    private http: HttpClient, 
    public activeRoute: ActivatedRoute, 
    //private seriesHttp: SeriesHttpService, 
    private conMessage: ConMessageService,
    //private patientCollection: PatientCollection, 
    public roiHttp: RoiHttpService, 
    private storageService: StorageService, 
    private seriesHttpService: SeriesHttpService
    ) {
      // this.activeRoute.queryParams.subscribe(params => {
      //     this.patientid = params['patientid'];
      // });
  }
  transverseChange(event: any) {
      if (event[2] == 'ver') {
          this.picLeft3.getBuffer(event[0], 'saggital', event[1])
          this.picLeft2.patient2screen(event[0]);
      }
      if (event[2] == 'cur') {
          this.picLeft2.getBuffer(event[0], 'coronal', event[1])
          this.picLeft3.patient2screen(event[0]);
      }
      if (event[2] == 'cro') {
          this.picLeft2.patient2screen(event[0]);
          this.picLeft3.patient2screen(event[0]);
          this.picLeft3.getBuffer(event[0], 'saggital', event[1])
          this.picLeft2.getBuffer(event[0], 'coronal', event[1])

      }
  }

  mainClearPri()
  {
      this.picLeft1.clearPri();
      this.picLeft2.clearPri();
      this.picLeft3.clearPri();
  }

showDialog() {
    this.display = true;
}
hideDialog() {
    this.display = false;
}
  coronalChange(event: any) {
      if (event[2] == 'ver') {
          this.picLeft3.getBuffer(event[0], 'saggital', event[1])
          this.picLeft1.patient2screen(event[0]);
      }
      if (event[2] == 'cur') {
          this.picLeft1.getBuffer(event[0], 'transverse', event[1])
          this.picLeft3.patient2screen(event[0]);
      }
      if (event[2] == 'cro') {
          this.picLeft3.patient2screen(event[0]);
          this.picLeft1.patient2screen(event[0]);
          this.picLeft1.getBuffer(event[0], 'transverse', event[1])
          this.picLeft3.getBuffer(event[0], 'saggital', event[1])
      }
  }
  saggitalChange(event: any) {
      if (event[2] == 'ver') {
          this.picLeft2.getBuffer(event[0], 'coronal', event[1])
          this.picLeft1.patient2screen(event[0]);
      }
      if (event[2] == 'cur') {
          this.picLeft1.getBuffer(event[0], 'transverse', event[1])
          this.picLeft2.patient2screen(event[0]);
      }
      if (event[2] == 'cro') {
          this.picLeft2.patient2screen(event[0]);
          this.picLeft1.patient2screen(event[0]);
          this.picLeft1.getBuffer(event[0], 'transverse', event[1])
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
  mainQuitDraw()
  {
 
  }

  ngOnInit() {
      if (this.conMessage.seriList != undefined) {
          this.seriList = this.conMessage.seriList[0];
      }
      this.conMessage.seriList$.subscribe(value => {
          this.seriList = value;
      });
      this.conMessage.curAction$.subscribe(value=>{
          this.action = value;
          if(this.action=="quitDrawPri"){
              this.picLeft1.SetCanvasIndex("#crossCanvas",4);
              this.picLeft2.SetCanvasIndex("#crossCanvas",4);
              this.picLeft3.SetCanvasIndex("#crossCanvas",4);
              this.picLeft1.SetCanvasIndex("#primitiveDrawCanvas",3);
              this.picLeft2.SetCanvasIndex("#primitiveDrawCanvas",3);
              this.picLeft3.SetCanvasIndex("#primitiveDrawCanvas",3);
          }
          else{
            this.picLeft1.SetCanvasIndex("#crossCanvas",3);
            this.picLeft2.SetCanvasIndex("#crossCanvas",3);
            this.picLeft3.SetCanvasIndex("#crossCanvas",3);
            this.picLeft1.SetCanvasIndex("#primitiveDrawCanvas",4);
            this.picLeft2.SetCanvasIndex("#primitiveDrawCanvas",4);
            this.picLeft3.SetCanvasIndex("#primitiveDrawCanvas",4);
          }
      }
      )
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

  mainHideList(){
    document.getElementById("series_list").classList.toggle("series_inactive");
    // document.getElementById("threebmp").classList.toggle("three_inactive");
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
  
  sliceX: any; sliceY: any; sliceZ: any; gap: any; sliceAll: any;
  /**
   * 加载序列响应函数，暂时向service随便发一个seriesId
   * @param seriesId 
   */
loadSeries() {
    $('#loading').showLoading();
    let img1 = new Image();
    let img2 = new Image();
    let img3 = new Image();
    let transverseCanvas = $(".a_class .icanvas").get(0);
    let saggitalCanvas = $(".c_class .icanvas").get(0);
    let coronalCanvas = $(".b_class .icanvas").get(0);
    let seriesId:any = "1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373";
    this.seriesHttpService.LoadSeries(seriesId).subscribe(value=>{
        if(value == "success")
        {
            this.seriesHttpService.GetSeries(seriesId,"","transverse",transverseCanvas.width,transverseCanvas.height).subscribe((value) =>{
                let data = JSON.parse(value);
                let base64Header = "data:image/png;base64,";
                let imgData:any;
                console.log("received transverse"); 
                if (data.transverse != null)
                {
                    console.log("pre transverse"); 
                    imgData = base64Header + data.transverse;
                    img1.src = imgData;
                    img1.onload = function(){
                        transverseCanvas.getContext("2d").clearRect(0,0,transverseCanvas.width,transverseCanvas.height);
                        transverseCanvas.getContext("2d").drawImage(img1,0,0,transverseCanvas.width,transverseCanvas.height);
                        console.log("onload transverse");    
                    }
                }
                $('#loading').hideLoading();
            },(error)=>{
                $('#loading').hideLoading();
                console.log(error);
            })
            this.seriesHttpService.GetSeries(seriesId,"","coronal",coronalCanvas.width,coronalCanvas.height).subscribe((value) =>{
                let data = JSON.parse(value);
                let base64Header = "data:image/png;base64,";
                let imgData:any;
                console.log("received coronal");
                if (data.coronal != null)
                {
                    console.log("pre coronal"); 
                    imgData = base64Header + data.coronal;
                    img2.src = imgData;
                    img2.onload = function(){
                        coronalCanvas.getContext("2d").clearRect(0,0,coronalCanvas.width,coronalCanvas.height);
                        coronalCanvas.getContext("2d").drawImage(img2,0,0,coronalCanvas.width,coronalCanvas.height);
                        console.log("onload coronal");     
                    }
                }
                $('#loading').hideLoading();
            },(error)=>{
                $('#loading').hideLoading();
                console.log(error);
            })
            this.seriesHttpService.GetSeries(seriesId,"","saggital",saggitalCanvas.width,saggitalCanvas.height).subscribe((value) =>{
                let data = JSON.parse(value);
                let base64Header = "data:image/png;base64,";
                let imgData:any;
                console.log("received saggital");
                if (data.saggital != null)
                {
                    console.log("pre saggital");
                    imgData = base64Header + data.saggital;
                    img3.src = imgData;
                    img3.onload = function(){
                        saggitalCanvas.getContext("2d").clearRect(0,0,saggitalCanvas.width,saggitalCanvas.height);
                        saggitalCanvas.getContext("2d").drawImage(img3,0,0,saggitalCanvas.width,saggitalCanvas.height);
                        console.log("onload saggital");     
                    }
                }
                $('#loading').hideLoading();
            },(error)=>{
                $('#loading').hideLoading();
                console.log(error);
            })
            console.log("wait for response")
            // let img1 = new Image();
            // let img2 = new Image();
            // let img3 = new Image();
            // this.seriesHttpService.GetSeries(seriesId,"","coronal",400,400).subscribe((value) =>{
            //     let data = JSON.parse(value);
            //     let base64Header = "data:image/png;base64,";
            //     let c1,c2,c3:any;
            //     let ctx1,ctx2,ctx3:any;
            //     let imgData1,imgData2,imgData3:any;
            //     if (data.transverse != null)
            //     {
            //         imgData1 = base64Header + data.transverse;
            //         img1.src = imgData1;
            //         img1.onload = function(){
            //             c1= $(".a_class .icanvas").get(0);
            //             ctx1=c1.getContext("2d");    
            //             ctx1.drawImage(img1,20,20,1000,650);
            //         }
            //     }
            //     if (data.saggital != null)
            //     {
            //         imgData2 = base64Header + data.saggital;
            //         img2.src = imgData2;
            //         img2.onload = function(){
            //             c2= $(".b_class .icanvas").get(0);
            //             ctx2=c2.getContext("2d");    
            //             ctx2.drawImage(img2,20,20,500,300);
            //         }
            //     }
            //     if (data.coronal != null)
            //     {
            //         imgData3 = base64Header + data.coronal;
            //         img3.src = imgData3;
            //         img3.onload = function(){
            //             c3= $(".c_class .icanvas").get(0);
            //             ctx3=c3.getContext("2d");   
            //             ctx3.drawImage(img3,20,20,500,300);
            //         }
            //     }
            // },(error)=>{
            //     console.log(error);
            // })
        }
    })
    console.log("wait for load response")
}
loadSeries1() {
    $('#loading').showLoading();
    let img1 = new Image();
    let img2 = new Image();
    let img3 = new Image();
    let transverseCanvas = $(".a_class .icanvas").get(0);
    let saggitalCanvas = $(".c_class .icanvas").get(0);
    let coronalCanvas = $(".b_class .icanvas").get(0);
    let seriesId:any = "1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373";
    this.seriesHttpService.GetSeries(seriesId,"","",400,400).subscribe((value) =>{
            let data = JSON.parse(value);
            let base64Header = "data:image/png;base64,";
            let c1,c2,c3:any;
            let ctx1,ctx2,ctx3:any;
            let imgData1,imgData2,imgData3:any;
            if (data.transverse != null)
                {
                    imgData1 = base64Header + data.transverse;
                    img1.src = imgData1;
                    img1.onload = function(){
                        c1= $(".a_class .icanvas").get(0);
                        ctx1=c1.getContext("2d");    
                        ctx1.drawImage(img1,20,20,1000,650);
                    }
            }
            if (data.saggital != null)
                {
                    imgData2 = base64Header + data.saggital;
                    img2.src = imgData2;
                    img2.onload = function(){
                        c2= $(".b_class .icanvas").get(0);
                        ctx2=c2.getContext("2d");    
                        ctx2.drawImage(img2,20,20,500,300);
                    }
            }
            if (data.coronal != null)
                {
                    imgData3 = base64Header + data.coronal;
                    img3.src = imgData3;
                    img3.onload = function(){
                        c3= $(".c_class .icanvas").get(0);
                        ctx3=c3.getContext("2d");   
                        ctx3.drawImage(img3,20,20,500,300);
                    }
            }
            $('#loading').hideLoading();
            },(error)=>{
                console.log(error);
                $('#loading').hideLoading();
            })
}
loadSeries2() {
    $('#loading').showLoading();
    let img1 = new Image();
    let img2 = new Image();
    let img3 = new Image();
    let series$:any;
    let transverseCanvas = $(".a_class .icanvas").get(0);
    let saggitalCanvas = $(".c_class .icanvas").get(0);
    let coronalCanvas = $(".b_class .icanvas").get(0);
    let seriesId:any = "1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373";
    this.seriesHttpService.LoadSeries(seriesId).subscribe(value=>{
        if(value == "success")
        {
            this.seriesHttpService.GetSeries(seriesId,"","transverse",transverseCanvas.width,transverseCanvas.height);
            this.seriesHttpService.GetSeries(seriesId,"","coronal",coronalCanvas.width,coronalCanvas.height);
            series$ = this.seriesHttpService.GetSeries(seriesId,"","saggital",saggitalCanvas.width,saggitalCanvas.height);
            series$.subscribe((value) =>{
                let data = JSON.parse(value);
                let base64Header = "data:image/png;base64,";
                let imgData:any;
                if (data.transverse != null)
                {
                    console.log("received transverse"); 
                    imgData = base64Header + data.transverse;
                    img1.src = imgData;
                    img1.onload = function(){
                        transverseCanvas.getContext("2d").clearRect(0,0,transverseCanvas.width,transverseCanvas.height);
                        transverseCanvas.getContext("2d").drawImage(img1,0,0,transverseCanvas.width,transverseCanvas.height);
                        console.log("onload transverse");    
                    }
                }
                if (data.coronal != null)
                {
                    console.log("received coronal"); 
                    imgData = base64Header + data.coronal;
                    img2.src = imgData;
                    img2.onload = function(){
                        coronalCanvas.getContext("2d").clearRect(0,0,coronalCanvas.width,coronalCanvas.height);
                        coronalCanvas.getContext("2d").drawImage(img2,0,0,coronalCanvas.width,coronalCanvas.height);
                        console.log("onload coronal");     
                    }
                }
                if (data.saggital != null)
                {
                    console.log("received saggital"); 
                    imgData = base64Header + data.saggital;
                    img3.src = imgData;
                    img3.onload = function(){
                        saggitalCanvas.getContext("2d").clearRect(0,0,saggitalCanvas.width,saggitalCanvas.height);
                        saggitalCanvas.getContext("2d").drawImage(img3,0,0,saggitalCanvas.width,saggitalCanvas.height);
                        console.log("onload saggital");     
                    }
                }
                $('#loading').hideLoading();
            },(error)=>{
                $('#loading').hideLoading();
                console.log(error);
            })
        }
    })
}
auto(node: any) {
      // var formData = { patientID: this.patientCollection.patient.Get(0).id, algorithmName: node[0], seriesID: this.patientCollection.patient.Get(0).series.Get(0).id };
      // this.roiHttp.PostCreateRoiByAtlas(formData).subscribe(value => {
      //     this.conMessage.SetRois(value.roiProperties);
      //     this.conMessage.Setcontour(value.roiGeometry.roiContourSets.items);
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
