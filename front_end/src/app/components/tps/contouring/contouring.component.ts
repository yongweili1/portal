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
  hasLoadVolume:boolean= false;
  seriesId:any;
  transverseCanvas:any;
  saggitalCanvas:any;
  coronalCanvas:any;
  
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
        this.picLeft2.cellUpdate(data['1']['image'], data['1']['crosshair']);
        this.picLeft3.cellUpdate(data['2']['image'], data['2']['crosshair']);
    }
    )
    }

  coronalChange(event: any) {
    let displayView = 'transverse,saggital'  
    this.seriesHttpService.GetLocatePic('coronal',displayView,event).subscribe((value)=>{
        let data = JSON.parse(value);
        this.picLeft1.cellUpdate(data['0']['image'], data['0']['crosshair']);
        this.picLeft3.cellUpdate(data['2']['image'], data['2']['crosshair']);
    }
    )
    }
  saggitalChange(event: any) {
    let displayView = 'transverse,coronal'
    this.seriesHttpService.GetLocatePic('saggital',displayView,event).subscribe((value)=>{
        let data = JSON.parse(value);
        this.picLeft1.cellUpdate(data['0']['image'], data['0']['crosshair']);
        this.picLeft2.cellUpdate(data['1']['image'], data['1']['crosshair']);
    })
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
    let data = {}
    if(this.hasLoadVolume == true){
        this.seriesHttpService.GetSeriesPic('transverse', 'transverse', event, "", "").subscribe((value) => {
        data = JSON.parse(value);
        this.picLeft1.drawCanvasPic(data['0']['image']);
        this.picLeft2.cross(data['1']['crosshair'][0],data['1']['crosshair'][1],1);
        this.picLeft3.cross(data['2']['crosshair'][0],data['2']['crosshair'][1],1);
    }, (error) => {
        console.log(error);
    })
    } 
  } 
  bCross(event: any) {
    let data = {}
    if(this.hasLoadVolume == true){
        this.seriesHttpService.GetSeriesPic('coronal', 'coronal', event, "", "").subscribe((value) => {
        data = JSON.parse(value);
        this.picLeft2.drawCanvasPic(data['1']['image']);
        this.picLeft1.cross(data['0']['crosshair'][0],data['0']['crosshair'][1],1);
        this.picLeft3.cross(data['2']['crosshair'][0],data['2']['crosshair'][1],1);
    }, (error) => {
        console.log(error);
    })
    } 
  }
  cCross(event: any) {
    let data = {}
    if(this.hasLoadVolume == true){
        this.seriesHttpService.GetSeriesPic('saggital', 'saggital', event, "", "").subscribe((value) => {
        data = JSON.parse(value);
        this.picLeft3.drawCanvasPic(data['2']['image']);
        this.picLeft2.cross(data['1']['crosshair'][0],data['1']['crosshair'][1],1);
        this.picLeft1.cross(data['0']['crosshair'][0],data['0']['crosshair'][1],1);
    }, (error) => {
        console.log(error);
    })
    } 
  }
  mainQuitDraw()
  {
 
  }

  ngOnInit() {      
      this.transverseCanvas = $(".a_class .icanvas").get(0);
      this.saggitalCanvas = $(".c_class .icanvas").get(0);
      this.coronalCanvas = $(".b_class .icanvas").get(0);
      if (this.conMessage.seriList != undefined) {
          this.seriList = this.conMessage.seriList[0];
      }
      this.conMessage.seriList$.subscribe(value => {
          this.seriList = value;
      });
      this.conMessage.curAction$.subscribe(value=>{
          this.action = value;
          if(this.action=="croselect"){
              this.picLeft1.SetCanvasIndex("#crossCanvas",10);
              this.picLeft2.SetCanvasIndex("#crossCanvas",10);
              this.picLeft3.SetCanvasIndex("#crossCanvas",10);
              this.picLeft1.SetCanvasIndex("#primitiveDrawCanvas",9);
              this.picLeft2.SetCanvasIndex("#primitiveDrawCanvas",9);
              this.picLeft3.SetCanvasIndex("#primitiveDrawCanvas",9);
          }
          else{
            this.picLeft1.SetCanvasIndex("#crossCanvas",9);
            this.picLeft2.SetCanvasIndex("#crossCanvas",9);
            this.picLeft3.SetCanvasIndex("#crossCanvas",9);
            this.picLeft1.SetCanvasIndex("#primitiveDrawCanvas",10);
            this.picLeft2.SetCanvasIndex("#primitiveDrawCanvas",10);
            this.picLeft3.SetCanvasIndex("#primitiveDrawCanvas",10);
          }
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
      $(window).resize(function() {
        setTimeout(()=>{
            if(that.hasLoadVolume == true){
                canvasSize['view_size'] = that.getCanvasSize();
                that.conService.noticeSize(canvasSize).subscribe();
            }
        },300);
    });
  }

  getCanvasSize(){
    let view_size :any = {};
    view_size['transverse'] = [this.transverseCanvas.width,this.transverseCanvas.height];
    view_size['coronal'] = [this.coronalCanvas.width,this.coronalCanvas.height];
    view_size['saggital'] = [this.saggitalCanvas.width,this.saggitalCanvas.height];

    return view_size;

  }

  mainHideList(){
    document.getElementById("series_list").classList.toggle("series_inactive");
  }
  mainzoom() {
      this.picLeft1.addZoomEvent();
      this.picLeft2.addZoomEvent();
      this.picLeft3.addZoomEvent();
  }
  mainZoomPro(evt){
      console.log(evt);
      let focus_view = evt[0];
      let factor = evt[1];
      let that = this;
      if(this.hasLoadVolume == true){
        this.seriesHttpService.GetZoomPic(focus_view,factor).subscribe(result =>{
            result = JSON.parse(result);
            that.picLeft1.cellUpdate(result['0']['image'], result['0']['crosshair']);
            that.picLeft2.cellUpdate(result['1']['image'], result['1']['crosshair']);
            that.picLeft3.cellUpdate(result['2']['image'], result['2']['crosshair']);
          })
      }
  }
  mainwlww() {
      this.picLeft1.changewl();
      this.picLeft2.changewl();
      this.picLeft3.changewl();
  }
  mainpan() {
      this.picLeft1.addPanEvent();
      this.picLeft2.addPanEvent();
      this.picLeft3.addPanEvent();
  }
  mainPanPro(evt){
    console.log(evt);
    let focus_view = evt[0];
    let prePos = evt[1];
    let curPso = evt[2];
    let that = this;
    if(this.hasLoadVolume == true){
      this.seriesHttpService.GetPanPic(focus_view,prePos,curPso).subscribe(result =>{
          result = JSON.parse(result);
          that.picLeft1.cellUpdate(result['0']['image'], result['0']['crosshair']);
          that.picLeft2.cellUpdate(result['1']['image'], result['1']['crosshair']);
          that.picLeft3.cellUpdate(result['2']['image'], result['2']['crosshair']);
        })
    }
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
    $('#loading').hideLoading();
    $('#loading').showLoading();
    //this.showDialog();
    let img1 = new Image();
    let img2 = new Image();
    let img3 = new Image();
    let transverseCanvas = $(".a_class .icanvas").get(0);
    let saggitalCanvas = $(".c_class .icanvas").get(0);
    let coronalCanvas = $(".b_class .icanvas").get(0);
    let seriesId:any = $("#seriesSelect").val();
    let canvasSize:any = {}
    canvasSize['view_size'] = this.getCanvasSize();
    console.log(canvasSize)
    this.seriesHttpService.LoadVolume(seriesId).subscribe(value=>{
        if(value == "success")
        {
                this.conService.noticeSize(canvasSize).subscribe(result=>{
                if(result.body == "success"){
                        this.seriesHttpService.GetSeries(seriesId,"","all",transverseCanvas.width,transverseCanvas.height).subscribe((value) =>{
                        let data = JSON.parse(value);
                        let base64Header = "data:image/png;base64,";
                        let imgData:any;
                        if (data['0']['image'] != null)
                        {
                            console.log("pre transverse"); 
                            imgData = base64Header + data['0']['image'];
                            img1.src = imgData;
                            img1.onload = function(){
                                transverseCanvas.getContext("2d").clearRect(0,0,transverseCanvas.width,transverseCanvas.height);
                                transverseCanvas.getContext("2d").drawImage(img1,0,0,transverseCanvas.width,transverseCanvas.height);
                                console.log("onload transverse");    
                            }
                        }
                        if (data['1']['image'] != null)
                        {
                            console.log("pre coronal"); 
                            imgData = base64Header + data['1']['image'];
                            img2.src = imgData;
                            img2.onload = function(){
                                coronalCanvas.getContext("2d").clearRect(0,0,coronalCanvas.width,coronalCanvas.height);
                                coronalCanvas.getContext("2d").drawImage(img2,0,0,coronalCanvas.width,coronalCanvas.height);
                                console.log("onload coronal");     
                            }
                        }
                        if (data['2']['image'] != null)
                        {
                            console.log("pre saggital");
                            imgData = base64Header + data['2']['image'];
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
                    this.hasLoadVolume = true;
                    this.seriesId = seriesId;
                }
            }
        
            );
        }
    })
  }

  fb(a) {
      this.load.loadbar(a)
  }

  message(w) {
      this.load.message(w);
  }
  ngOnDestroy(){
    this.hasLoadVolume = false;
    this.seriesHttpService.UnLoadVolume(this.seriesId).subscribe();
    $('#loading').hideLoading();
  }
}

