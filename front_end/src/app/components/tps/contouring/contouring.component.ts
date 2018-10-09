import { Component, OnInit, ViewChild, Injector, ElementRef } from '@angular/core';
import { ActivatedRoute, ParamMap } from '@angular/router';
import { HttpClient } from "@angular/common/http";
import { DialogModule } from 'primeng/dialog';

import { ConMessageService } from '../shared/service/conMessage.service';
//import { PatientCollection } from '../shared/PatientCollection';
import { SeriesHttpService } from '../shared/service/seriesHttp.service';
import { RoiHttpService } from '../shared/service/roiHttp.service';
import { StorageService } from '../shared/service/storage.service';

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

      }
  }

  mainClearPri()
  {
      this.picLeft1.clearPri();
      this.picLeft2.clearPri();
      this.picLeft3.clearPri();
  }
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
    this.seriesHttpService.GetSeriesPic(seriesId)
        .subscribe((value) =>{
        // img.src = data;
        // img.onload = function(){
        //     let c= $(".a_class .icanvas").get(0);
        //     let ctx=c.getContext("2d");    
        //     ctx.drawImage(img,20,20,1000,650);
    
        //     c= $(".b_class .icanvas").get(0);
        //     ctx=c.getContext("2d");
        //     ctx.drawImage(img,20,20,500,300);
    
        //     c= $(".c_class .icanvas").get(0);
        //     ctx=c.getContext("2d");
        //     ctx.drawImage(img,20,20,500,300);
       
        // }
        let data = JSON.parse(value);
        if (data.saggital != null)
            {
                let saggital_canvas = $(".c_class .icanvas").get(0);
                let saggital_ctx = saggital_canvas.getContext("2d");
                saggital_ctx.clearRect(0,0,saggital_canvas.width,saggital_canvas.height);
                let saggital_img = saggital_ctx.createImageData(data.saggital.length,data.saggital[0].length);
                for (let i = 0; i < data.saggital.length; i++){
                    for (let j = 0; j < data.saggital.length; j++) {
                        saggital_img.data[4 * data.saggital.length * i + j * 4] = data.saggital[i][j][0];
                        saggital_img.data[4 * data.saggital.length * i + j * 4 + 1] = data.saggital[i][j][1];
                        saggital_img.data[4 * data.saggital.length * i + j * 4 + 2] = data.saggital[i][j][2];
                        saggital_img.data[4 * data.saggital.length * i + j * 4 + 3] = 255;
                    }
                }
                saggital_ctx.putImageData(saggital_img, (saggital_canvas.width-data.saggital.length)/2,(saggital_canvas.height-data.saggital[0].length)/2, 0, 0,saggital_canvas.width,saggital_canvas.height);
            }

        if (data.coronal != null)
            {
                let coronal_canvas = $(".b_class .icanvas").get(0);
                let coronal_ctx = coronal_canvas.getContext("2d");
                coronal_ctx.clearRect(0,0,coronal_canvas.width,coronal_canvas.height);
                let coronal_img = coronal_ctx.createImageData(data.coronal.length,data.coronal[0].length);
                for (let i = 0; i < data.coronal.length; i++){
                    for (let j = 0; j < data.coronal.length; j++) {
                        coronal_img.data[4 * data.coronal.length * i + j * 4] = data.coronal[i][j][0];
                        coronal_img.data[4 * data.coronal.length * i + j * 4 + 1] = data.coronal[i][j][1];
                        coronal_img.data[4 * data.coronal.length * i + j * 4 + 2] = data.coronal[i][j][2];
                        coronal_img.data[4 * data.coronal.length * i + j * 4 + 3] = 255;
                    }
                }
                coronal_ctx.putImageData(coronal_img,(coronal_canvas.width-data.coronal.length)/2,(coronal_canvas.height-data.coronal[0].length)/2, 0, 0,coronal_canvas.width,coronal_canvas.height);
            }
        if (data.transverse != null)
        {
            let transverse_canvas = $(".a_class .icanvas").get(0);
            let transverse_ctx = transverse_canvas.getContext("2d");
            transverse_ctx.clearRect(0,0,transverse_canvas.width,transverse_canvas.height);
            let transverse_img = transverse_ctx.createImageData(data.transverse.length,data.transverse[0].length);
            for (let i = 0; i < data.transverse.length; i++){
                for (let j = 0; j < data.transverse[0].length; j++) {
                    transverse_img.data[4 * data.transverse.length * i + j * 4] = data.transverse[i][j][0];
                    transverse_img.data[4 * data.transverse.length * i + j * 4 + 1] = data.transverse[i][j][1];
                    transverse_img.data[4 * data.transverse.length * i + j * 4 + 2] = data.transverse[i][j][2];
                    transverse_img.data[4 * data.transverse.length * i + j * 4 + 3] = 255;
                }
            }
            //transverse_img.width = transverse_canvas.width;
            //transverse_ctx.drawImage(transverse_img,0,0,transverse_canvas.width,transverse_canvas.height);
            transverse_ctx.putImageData(transverse_img,(transverse_canvas.width-data.transverse.length)/2,(transverse_canvas.height-data.transverse[0].length)/2, 0, 0,transverse_canvas.width,transverse_canvas.height);
        }
    },(error)=>{
        console.log(error);
    })
    let dd=0;
    // $.ajax({
    //     type: "GET",
    //     url: "http://127.0.0.1:8000/image/images/?seriesuid=1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373&width=400&height=400&focus_view=&display_view=",
    //  }).done(function (data, a, b, c) {
    //     // img.src = response;
    //     // img.onload = function(){
    //     //     let c= $(".a_class .icanvas").get(0);
    //     //     let ctx=c.getContext("2d");
        
    //     //     ctx.drawImage(img,20,20,1000,650);
        
    //     //     c= $(".b_class .icanvas").get(0);
    //     //     ctx=c.getContext("2d");
    //     //     ctx.drawImage(img,20,20,500,300);
        
    //     //     c= $(".c_class .icanvas").get(0);
    //     //     ctx=c.getContext("2d");
    //     //     ctx.drawImage(img,20,20,500,300);
       
    //     // }

    //     data = JSON.parse(data);
    //     if (data.saggital != null)
    //         {
    //             let saggital_canvas = $(".c_class .icanvas").get(0);
    //             let saggital_ctx = saggital_canvas.getContext("2d");
    //             saggital_ctx.clearRect(0,0,saggital_canvas.width,saggital_canvas.height);
    //             let saggital_img = saggital_ctx.createImageData(data.saggital.length,data.saggital[0].length);
    //             for (let i = 0; i < data.saggital.length; i++){
    //                 for (let j = 0; j < data.saggital.length; j++) {
    //                     saggital_img.data[4 * data.saggital.length * i + j * 4] = data.saggital[i][j][0];
    //                     saggital_img.data[4 * data.saggital.length * i + j * 4 + 1] = data.saggital[i][j][1];
    //                     saggital_img.data[4 * data.saggital.length * i + j * 4 + 2] = data.saggital[i][j][2];
    //                     saggital_img.data[4 * data.saggital.length * i + j * 4 + 3] = 255;
    //                 }
    //             }
    //             saggital_ctx.putImageData(saggital_img, (saggital_canvas.width-data.saggital.length)/2,(saggital_canvas.height-data.saggital[0].length)/2, 0, 0,saggital_canvas.width,saggital_canvas.height);
    //         }

    //     if (data.coronal != null)
    //         {
    //             let coronal_canvas = $(".b_class .icanvas").get(0);
    //             let coronal_ctx = coronal_canvas.getContext("2d");
    //             coronal_ctx.clearRect(0,0,coronal_canvas.width,coronal_canvas.height);
    //             let coronal_img = coronal_ctx.createImageData(data.coronal.length,data.coronal[0].length);
    //             for (let i = 0; i < data.coronal.length; i++){
    //                 for (let j = 0; j < data.coronal.length; j++) {
    //                     coronal_img.data[4 * data.coronal.length * i + j * 4] = data.coronal[i][j][0];
    //                     coronal_img.data[4 * data.coronal.length * i + j * 4 + 1] = data.coronal[i][j][1];
    //                     coronal_img.data[4 * data.coronal.length * i + j * 4 + 2] = data.coronal[i][j][2];
    //                     coronal_img.data[4 * data.coronal.length * i + j * 4 + 3] = 255;
    //                 }
    //             }
    //             coronal_ctx.putImageData(coronal_img,(coronal_canvas.width-data.coronal.length)/2,(coronal_canvas.height-data.coronal[0].length)/2, 0, 0,coronal_canvas.width,coronal_canvas.height);
    //         }
    //     if (data.transverse != null)
    //     {
    //         let transverse_canvas = $(".a_class .icanvas").get(0);
    //         let transverse_ctx = transverse_canvas.getContext("2d");
    //         transverse_ctx.clearRect(0,0,transverse_canvas.width,transverse_canvas.height);
    //         let transverse_img = transverse_ctx.createImageData(data.transverse.length,data.transverse[0].length);
    //         for (let i = 0; i < data.transverse.length; i++){
    //             for (let j = 0; j < data.transverse[0].length; j++) {
    //                 transverse_img.data[4 * data.transverse.length * i + j * 4] = data.transverse[i][j][0];
    //                 transverse_img.data[4 * data.transverse.length * i + j * 4 + 1] = data.transverse[i][j][1];
    //                 transverse_img.data[4 * data.transverse.length * i + j * 4 + 2] = data.transverse[i][j][2];
    //                 transverse_img.data[4 * data.transverse.length * i + j * 4 + 3] = 255;
    //             }
    //         }
    //         //transverse_img.width = transverse_canvas.width;
    //         //transverse_ctx.drawImage(transverse_img,0,0,transverse_canvas.width,transverse_canvas.height);
    //         transverse_ctx.putImageData(transverse_img,(transverse_canvas.width-data.transverse.length)/2,(transverse_canvas.height-data.transverse[0].length)/2, 0, 0,transverse_canvas.width,transverse_canvas.height);
    //     }
    //  }).fail(function (jqXHR, textStatus, errorThrown) {
    //     alert(jqXHR.responseJSON.error.message.value);
    //  });

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
