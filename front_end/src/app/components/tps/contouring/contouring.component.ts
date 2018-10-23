import { Component, OnInit, ViewChild, Injector, ElementRef } from '@angular/core';
import { ActivatedRoute, ParamMap } from '@angular/router';
import { HttpClient } from "@angular/common/http";
import { switchMap } from 'rxjs/operators';
import { Router} from '@angular/router';
import { ElMessageService } from 'element-angular'

import { ConMessageService } from '../shared/service/conMessage.service';
import { SeriesHttpService } from '../shared/service/seriesHttp.service';
import { RoiHttpService } from '../shared/service/roiHttp.service';
import { StorageService } from '../shared/service/storage.service';
import { ContouringService } from '../shared/service/contouring.service';


import {
  LazyLoadEvent, ConfirmationService, Paginator
} from 'primeng/primeng';

import { ToastService } from '../../../core/toast.service';
import { Page, PageRequest } from '../../../shared/models';

declare var $: any;

@Component({
  selector: 'mpt-contouring',
  templateUrl: './contouring.component.html',
  styleUrls: ['./contouring.component.less']
})
export class ContouringComponent implements OnInit {
  patientId: any = "";
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
  seriesList:any;
  
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
    public roiHttp: RoiHttpService, 
    private storageService: StorageService, 
    private seriesHttpService: SeriesHttpService,
    private router:Router,
    private conService:ContouringService,
    private elMessage: ElMessageService,
    ) {
  }
  transverseChange(event: any) {
    let displayView = 'coronal,saggital'  
    this.seriesHttpService.GetLocatePic('transverse',displayView,event).subscribe((value)=>{
        let data = JSON.parse(value);
        this.picLeft2.locateUpdate(data.coronal, data.cross_position);
        this.picLeft3.locateUpdate(data.saggital, data.cross_position);
    }
    )
  }

  coronalChange(event: any) {
    let displayView = 'transverse,saggital'  
    this.seriesHttpService.GetLocatePic('coronal',displayView,event).subscribe((value)=>{
        let data = JSON.parse(value);
        this.picLeft1.locateUpdate(data.transverse, data.cross_position);
        this.picLeft3.locateUpdate(data.saggital, data.cross_position);
    }
    )
}
  saggitalChange(event: any) {
    let displayView = 'coronal,transverse'
    this.seriesHttpService.GetLocatePic('saggital',displayView,event).subscribe((value)=>{
        let data = JSON.parse(value);
        this.picLeft1.locateUpdate(data.transverse, data.cross_position);
        this.picLeft2.locateUpdate(data.coronal, data.cross_position);
    }
    )
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


  aCross(event: any) {
      
    //   this.picLeft2.cross(event.cronPosition[0],event.cronPosition[1],1);
    //   this.picLeft3.cross(event.sagPosition[0],event.sagPosition[1],1);
      this.sendCrossInfo('cronal',  [event.cronPosition[0],event.cronPosition[1]])
      this.sendCrossInfo('saggital',  [event.sagPosition[0],event.sagPosition[1]])
  }
  bCross(event: any) {
    //   this.picLeft1.cross(event.transPosition[0],event.transPosition[1],1);
    //   this.picLeft3.cross(event.sagPosition[0],event.sagPosition[1],1);
      this.sendCrossInfo('transerval',  [event.transPosition[0],event.transPosition[1]])
      this.sendCrossInfo('saggital',  [event.sagPosition[0],event.sagPosition[1]])
  }
  cCross(event: any) {
    //   this.picLeft2.cross(event.cronPosition[0],event.cronPosition[1],1);
    //   this.picLeft1.cross(event.transPosition[0],event.transPosition[1],1);
    this.sendCrossInfo('cronal', [event.cronPosition[0],event.cronPosition[1]])
    this.sendCrossInfo('transerval', [event.transPosition[0],event.transPosition[1]])
  }
  sendCrossInfo(viewName, point) {
    let crossInfo = {}
    crossInfo['view'] = viewName
    crossInfo['point'] = point
    this.conMessage.setCrossPoint(crossInfo)
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
        //   if(this.action=="quitDrawPri"){
        //       this.picLeft1.SetCanvasIndex("#crossCanvas",4);
        //       this.picLeft2.SetCanvasIndex("#crossCanvas",4);
        //       this.picLeft3.SetCanvasIndex("#crossCanvas",4);
        //       this.picLeft1.SetCanvasIndex("#primitiveDrawCanvas",3);
        //       this.picLeft2.SetCanvasIndex("#primitiveDrawCanvas",3);
        //       this.picLeft3.SetCanvasIndex("#primitiveDrawCanvas",3);
        //       this.picLeft1.SetCanvasIndex("#primitiveCanvas",2);
        //       this.picLeft2.SetCanvasIndex("#primitiveCanvas",2);
        //       this.picLeft3.SetCanvasIndex("#primitiveCanvas",2);
        //   }
        //   else if(this.action=="select"){
        //       this.picLeft1.SetCanvasIndex("#primitiveCanvas",55);
        //       this.picLeft2.SetCanvasIndex("#primitiveCanvas",55);
        //       this.picLeft3.SetCanvasIndex("#primitiveCanvas",55);
        //   }
        //   else{
        //     this.picLeft1.SetCanvasIndex("#crossCanvas",3);
        //     this.picLeft2.SetCanvasIndex("#crossCanvas",3);
        //     this.picLeft3.SetCanvasIndex("#crossCanvas",3);
        //     this.picLeft1.SetCanvasIndex("#primitiveDrawCanvas",4);
        //     this.picLeft2.SetCanvasIndex("#primitiveDrawCanvas",4);
        //     this.picLeft3.SetCanvasIndex("#primitiveDrawCanvas",4);
        //     this.picLeft1.SetCanvasIndex("#primitiveCanvas",2);
        //     this.picLeft2.SetCanvasIndex("#primitiveCanvas",2);
        //     this.picLeft3.SetCanvasIndex("#primitiveCanvas",2);
        //   }
      });
      this.activeRoute.queryParams.subscribe(params => {
        this.patientId = params.patientId;
        if(this.patientId !="" &&this.patientId != undefined &&this.patientId !=null){
            this.getSeriesList(this.patientId);
        }
        else{
            this.elMessage.setOptions({ showClose: true,customClass:"elmessage" })
            this.elMessage['error']('请先选择病人')
            //this.router.navigate(['/base/patient-template']);
        }
      });
      let that = this;
      let canvasSize:any = {};
      setTimeout(()=>{
            canvasSize['view_size'] = that.getCanvasSize();
            that.conService.noticeSize(canvasSize).subscribe();
        },300);
      $(window).resize(function() {
        setTimeout(()=>{
            canvasSize['view_size'] = that.getCanvasSize();
            //canvasSize = {'view_size':"cc"}
            that.conService.noticeSize(canvasSize).subscribe();
        },300);
    });
  }

  getCanvasSize(){
    let view_size :any = {};
    let transverseCanvas = $(".a_class .icanvas").get(0);
    let saggitalCanvas = $(".c_class .icanvas").get(0);
    let coronalCanvas = $(".b_class .icanvas").get(0);
    view_size['transverse'] = [transverseCanvas.width,transverseCanvas.height];
    view_size['coronal'] = [coronalCanvas.width,coronalCanvas.height];
    view_size['saggital'] = [saggitalCanvas.width,saggitalCanvas.height];

    return view_size;

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

  getSeriesList(patientId:any){
    this.seriesHttpService.getSeriesList(patientId).subscribe(data=>{
        this.seriesList = data;
    })
  }
  
  sliceX: any; sliceY: any; sliceZ: any; gap: any; sliceAll: any;
  /**
   * 加载序列响应函数，暂时向service随便发一个seriesId
   * @param seriesId 
   */
  loadSeries() {
    $('#loading').showLoading();
    //this.showDialog();
    let img1 = new Image();
    let img2 = new Image();
    let img3 = new Image();
    let transverseCanvas = $(".a_class .icanvas").get(0);
    let saggitalCanvas = $(".c_class .icanvas").get(0);
    let coronalCanvas = $(".b_class .icanvas").get(0);
    let seriesId:any = $("#seriesSelect").val();
    this.seriesHttpService.LoadSeries(seriesId).subscribe(value=>{
        if(value == "success")
        {
            this.seriesHttpService.GetSeries(seriesId,"","transverse",transverseCanvas.width,transverseCanvas.height).subscribe((value) =>{
                let data = JSON.parse(value);
                let base64Header = "data:image/png;base64,";
                let imgData:any;
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
                //this.hideDialog();
            },(error)=>{
                $('#loading').hideLoading();
                //this.hideDialog();
                console.log(error);
            })
            this.seriesHttpService.GetSeries(seriesId,"","coronal",coronalCanvas.width,coronalCanvas.height).subscribe((value) =>{
                let data = JSON.parse(value);
                let base64Header = "data:image/png;base64,";
                let imgData:any;
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
                //this.hideDialog();
            },(error)=>{
                $('#loading').hideLoading();
                //this.hideDialog();
                console.log(error);
            })
            this.seriesHttpService.GetSeries(seriesId,"","saggital",saggitalCanvas.width,saggitalCanvas.height).subscribe((value) =>{
                let data = JSON.parse(value);
                let base64Header = "data:image/png;base64,";
                let imgData:any;
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
                //this.hideDialog();
            },(error)=>{
                $('#loading').hideLoading();
                //this.hideDialog();
                console.log(error);
            })
            console.log("wait for response")
        }
    })
    console.log("wait for load response")
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

