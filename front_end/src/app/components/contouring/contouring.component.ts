import { Component, OnInit, ViewChild } from '@angular/core';
import { ActivatedRoute } from '@angular/router';
import { MessageService } from 'primeng/api';
import { EventAggregator } from '../../shared/common/event_aggregator';
import { LazyExcuteHandler } from './lazy_excute_handler';
import { ContourDto } from './shared/dtos/contour_dto';
import { ROIConfig } from './shared/model/ROIConfig.model';
import { ConMessageService } from './shared/service/conMessage.service';
import { ContouringService } from './shared/service/contouring.service';
import { RoiHttpService } from './shared/service/roiHttp.service';
import { SeriesHttpService } from './shared/service/seriesHttp.service';

declare var $: any;
declare var actions: any;

@Component({
    selector: 'mpt-contouring',
    templateUrl: './contouring.component.html',
    styleUrls: ['./contouring.component.less'],
})
export class ContouringComponent implements OnInit {
    patientId: any = '';
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
    actionInfo: any;
    display = false;
    seriesList: any;
    hasLoadVolume = false;
    seriesId: any;
    transverseCanvas: any;
    saggitalCanvas: any;
    coronalCanvas: any;
    ROIName: any = '';
    ROIColor: any = '#FFFF00';
    ROIList: Array<any>;
    ROIListLength = 0;
    newROIDisplay: any = false;
    manageROIDisplay: any = false;
    editROIDisplay: any = false;
    editROIConfig: ROIConfig = { ROIId: '', ROIName: '', ROIColor: '' };
    activeROIConfig: ROIConfig = { ROIId: '', ROIName: '', ROIColor: '' };
    sliceIndex: any;
    lazyExcuteHandler: LazyExcuteHandler;

    @ViewChild('picLeft1') picLeft1;
    @ViewChild('picLeft2') picLeft2;
    @ViewChild('picLeft3') picLeft3;
    @ViewChild('load') load;

    constructor(
        public activeRoute: ActivatedRoute,
        private conMessage: ConMessageService,
        public roiHttp: RoiHttpService,
        private seriesHttpService: SeriesHttpService,
        private conService: ContouringService,
        private priMessageService: MessageService
    ) {
        this.lazyExcuteHandler = new LazyExcuteHandler();
        EventAggregator.Instance().contourCps.subscribe(data => { this.saveContour(data); });
        EventAggregator.Instance().removeCps.subscribe(data => { this.deleteContours(data); });
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

        EventAggregator.Instance().actionInfo.subscribe(value => {
            this.actionInfo = value;
            const priActionArray = ['shape', 'clear', 'select', 'nudge'];
            if (this.actionInfo.key() === actions.locate) {
                this.picLeft1.SetCanvasIndex("#crossCanvas", 10);
                this.picLeft2.SetCanvasIndex("#crossCanvas", 10);
                this.picLeft3.SetCanvasIndex("#crossCanvas", 10);
                this.picLeft1.SetCanvasIndex("#overlayCanvas", 9);
                this.picLeft2.SetCanvasIndex("#overlayCanvas", 9);
                this.picLeft3.SetCanvasIndex("#overlayCanvas", 9);
                this.picLeft1.SetCanvasIndex("#toolsCanvas", 8);
                this.picLeft2.SetCanvasIndex("#toolsCanvas", 8);
                this.picLeft3.SetCanvasIndex("#toolsCanvas", 8);
            } else if (priActionArray.indexOf(this.actionInfo.key()) > -1) {
                this.picLeft1.SetCanvasIndex("#crossCanvas", 9);
                this.picLeft2.SetCanvasIndex("#crossCanvas", 9);
                this.picLeft3.SetCanvasIndex("#crossCanvas", 9);
                this.picLeft1.SetCanvasIndex("#overlayCanvas", 10);
                this.picLeft2.SetCanvasIndex("#overlayCanvas", 10);
                this.picLeft3.SetCanvasIndex("#overlayCanvas", 10);
                this.picLeft1.SetCanvasIndex("#toolsCanvas", 8);
                this.picLeft2.SetCanvasIndex("#toolsCanvas", 8);
                this.picLeft3.SetCanvasIndex("#toolsCanvas", 8);
            } else {
                this.picLeft1.SetCanvasIndex("#crossCanvas", 9);
                this.picLeft2.SetCanvasIndex("#crossCanvas", 9);
                this.picLeft3.SetCanvasIndex("#crossCanvas", 9);
                this.picLeft1.SetCanvasIndex("#overlayCanvas", 8);
                this.picLeft2.SetCanvasIndex("#overlayCanvas", 8);
                this.picLeft3.SetCanvasIndex("#overlayCanvas", 8);
                this.picLeft1.SetCanvasIndex("#toolsCanvas", 10);
                this.picLeft2.SetCanvasIndex("#toolsCanvas", 10);
                this.picLeft3.SetCanvasIndex("#toolsCanvas", 10);
            }
        });
        this.activeRoute.queryParams.subscribe(params => {
            this.patientId = params.patientId;
        });
        const that = this;
        const canvasSize: any = {};
        $(window).resize(function () {
            setTimeout(() => {
                if (that.hasLoadVolume === true) {
                    canvasSize['view_size'] = that.getCanvasSize();
                    that.conService.noticeSize(canvasSize).subscribe(result => {
                        if (result.body === "success" && that.hasLoadVolume) {
                            that.seriesHttpService.GetSeries("", "", "all", "", "").subscribe(data => {
                                data = JSON.parse(data);
                                that.updateCells(result);
                                that.sliceIndex = data['0']['slice_index'];
                                that.conMessage.SetSliceIndex(that.sliceIndex);
                            });
                        }
                    });
                }
            }, 300);
        });
    }

    ngAfterViewInit() {
        if (this.patientId === '' || this.patientId === undefined || this.patientId == null) {
            this.priMessageService.add({ severity: 'error', detail: 'Please select the patient first.' });
        } else {
            this.getSeriesList(this.patientId);
        }
    }

    ngOnDestroy() {
        this.hasLoadVolume = false;
        this.seriesHttpService.UnLoadVolume(this.seriesId).subscribe();
    }

    mainNewROI() {
        const seriesuid = $("#seriesSelect").val();
        if (seriesuid != '' && seriesuid != null && seriesuid != undefined) {
            this.newROIDisplay = true;
        } else {
            this.priMessageService.add({ severity: 'error', detail: 'Please select series first.' });
        }
    }

    mainautoroi() {
        const ROIData = {
            seriesuid: $("#seriesSelect").val(),
            ROIName: 'heart',
            ROIColor: '#FFFF00'
        };
        this.roiHttp.CreateNewSegROI(ROIData).subscribe(result => {
            if (result.body.code == '200') {
                this.priMessageService.add({ severity: 'success', detail: `Save succeed.` });
                this.ROIList = result.body.data;
                this.ROIListLength = this.ROIList.length;
                this.newROIDisplay = false;
                this.ROIList.forEach(element => {
                    if (element.ROIId > 0) {
                        this.activeROIConfig = element;
                    }
                });

                this.conMessage.SetActiveRoi(this.activeROIConfig);
            } else {
                this.priMessageService.add({ severity: 'error', detail: `${result.msg}` });
            }
        })
    }

    mainManageROI() {
        let seriesuid = $("#seriesSelect").val();
        if (seriesuid != '' && seriesuid != null && seriesuid != undefined) {
            this.manageROIDisplay = true;
            this.roiHttp.GetROIConfig(seriesuid).subscribe(result => {
                if (result.code = '200') {
                    this.ROIList = result['data'];
                    this.ROIListLength = this.ROIList.length;
                }
            });
        } else {
            this.priMessageService.add({ severity: 'error', detail: 'Please select series first.' });
        }
    }

    hideNewROIDia() {
        this.ROIColor = '#FFFF00';
        this.ROIName = '';
    }

    saveROI() {
        if (this.ROIName == '') {
            this.priMessageService.add({ severity: 'error', detail: `Illegal input.` });
            return;
        }
        const ROIData = {
            seriesuid: $('#seriesSelect').val(),
            ROIName: this.ROIName,
            ROIColor: this.ROIColor
        };
        this.roiHttp.PostCreateNewROI(ROIData).subscribe(result => {
            if (result.body.code == '200') {
                this.priMessageService.add({ severity: 'success', detail: `Save succeed.` });
                this.ROIList = result.body.data;
                this.ROIListLength = this.ROIList.length;
                this.newROIDisplay = false;
                this.ROIList.forEach(element => {
                    if (element.ROIId > 0) {
                        this.activeROIConfig = element;
                    }
                });

                this.conMessage.SetActiveRoi(this.activeROIConfig);
            } else {
                this.priMessageService.add({ severity: 'error', detail: `${result.msg}` });
            }
        });
    }

    updateROI() {
        if (this.editROIConfig.ROIName == '') {
            this.priMessageService.add({ severity: 'error', detail: `Illegal input.` });
            return;
        }
        this.roiHttp.UpdateROIConfig(this.editROIConfig).subscribe(result => {
            if (result.body.code == '200') {
                this.priMessageService.add({ severity: 'success', detail: `Save succeed.` });
                this.ROIList = result.body.data;
                this.ROIListLength = this.ROIList.length;
                this.editROIDisplay = false;
            } else {
                this.priMessageService.add({ severity: 'error', detail: `${result.msg}` });
            }
        });
    }

    deleteROI(evt) {
        const ROIId = $(evt.target).parents('tr').find('.roi-id-td').text();
        this.roiHttp.DeleteROIConfig(ROIId).subscribe(result => {
            if (result.code == '200') {
                this.priMessageService.add({ severity: 'success', detail: `Delete succeed.` });
                this.ROIList = result.data;
                this.ROIListLength = this.ROIList.length;
                this.editROIDisplay = false;
            } else {
                this.priMessageService.add({ severity: 'error', detail: `${result.msg}` });
            }
        });
    }

    deleteAllROI() {
        const ROIIdArray = [];
        this.ROIList.forEach(element => {
            ROIIdArray.push(element.ROIId);
        });
        this.roiHttp.DeleteROIConfig(ROIIdArray).subscribe(result => {
            if (result.code == '200') {
                this.priMessageService.add({ severity: 'success', detail: `Delete succeed.` });
                this.ROIList = result.data;
                this.ROIListLength = 0;
            } else {
                this.priMessageService.add({ severity: 'error', detail: `${result.msg}` });
            }
        }
        )
    }

    hideManageROIDia() {

    }

    changeROIVisible(evt) {
        evt.target.classList.toggle("fa-eye-slash");
    }

    onClickROIItem(evt) {
        $(".roi-select-tr").removeClass('roi-select-tr');
        $(evt.target).parents('tr').addClass('roi-select-tr');
        this.activeROIConfig.ROIId = $(evt.target).parents('tr').find('.roi-id-td').text();
        this.ROIList.forEach(element => {
            if (this.activeROIConfig.ROIId == element.ROIId) {
                this.activeROIConfig.ROIName = element.ROIName;
                this.activeROIConfig.ROIColor = element.ROIColor;
            }
        });
        this.conMessage.SetActiveRoi(this.activeROIConfig);
    }

    editROI(evt) {
        this.editROIConfig.ROIId = $(evt.target).parents('tr').find('.roi-id-td').text();
        this.editROIDisplay = true;
        this.ROIList.forEach(element => {
            if (this.editROIConfig.ROIId == element.ROIId) {
                this.editROIConfig.ROIName = element.ROIName;
                this.editROIConfig.ROIColor = element.ROIColor;
            }
        });
    }

    transverseChange(event: any) {
        if (!this.hasLoadVolume) {
            return;
        }
        const displayView = 'coronal,saggital';
        this.seriesHttpService.GetLocatePic('transverse', displayView, event).subscribe((value) => {
            const data = JSON.parse(value);
            this.updateCells(data, false, displayView);
            this.sliceIndex = data['0']['slice_index'];
            this.conMessage.SetSliceIndex(this.sliceIndex);
            EventAggregator.Instance().sliceIndex.publish(this.sliceIndex);
        });
    }

    coronalChange(event: any) {
        if (!this.hasLoadVolume) {
            return;
        }
        const displayView = 'transverse,saggital';
        this.seriesHttpService.GetLocatePic('coronal', displayView, event).subscribe((value) => {
            const data = JSON.parse(value);
            this.updateCells(data, false, displayView);
            this.sliceIndex = data['0']['slice_index'];
            this.conMessage.SetSliceIndex(this.sliceIndex);
            EventAggregator.Instance().sliceIndex.publish(this.sliceIndex);
        });
    }

    saggitalChange(event: any) {
        if (!this.hasLoadVolume) {
            return;
        }
        const displayView = 'transverse,coronal';
        this.seriesHttpService.GetLocatePic('saggital', displayView, event).subscribe((value) => {
            const data = JSON.parse(value);
            this.updateCells(data, false, displayView);
            this.sliceIndex = data['0']['slice_index'];
            this.conMessage.SetSliceIndex(this.sliceIndex);
            EventAggregator.Instance().sliceIndex.publish(this.sliceIndex);
        });
    }

    mainClearPri() {
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
        if (!this.lazyExcuteHandler.canExcute(new Date().getTime(), 'a')
            || !this.hasLoadVolume) {
            return;
        }

        this.seriesHttpService.GetSeriesPic('transverse', 'transverse', event, '', '').subscribe((value) => {
            const data = JSON.parse(value);
            this.updateCells(data, false);
            this.sliceIndex = data['0']['slice_index'];
            this.conMessage.SetSliceIndex(this.sliceIndex);
            EventAggregator.Instance().sliceIndex.publish(this.sliceIndex);
        }, (error) => {
            console.log(error);
        });
    }

    bCross(event: any) {
        if (!this.lazyExcuteHandler.canExcute(new Date().getTime(), 'b') ||
            !this.hasLoadVolume) {
            return;
        }

        this.seriesHttpService.GetSeriesPic('coronal', 'coronal', event, '', '').subscribe((value) => {
            const data = JSON.parse(value);
            this.updateCells(data, false);
            this.sliceIndex = data['0']['slice_index'];
            EventAggregator.Instance().sliceIndex.publish(this.sliceIndex);
        }, (error) => {
            console.log(error);
        });
    }

    cCross(event: any) {
        if (!this.lazyExcuteHandler.canExcute(new Date().getTime(), 'c')
            || !this.hasLoadVolume) {
            return;
        }

        this.seriesHttpService.GetSeriesPic('saggital', 'saggital', event, '', '').subscribe((value) => {
            const data = JSON.parse(value);
            this.updateCells(data, false);
            this.sliceIndex = data['0']['slice_index'];
            EventAggregator.Instance().sliceIndex.publish(this.sliceIndex);
        }, (error) => {
            console.log(error);
        });
    }

    mainQuitDraw() {

    }

    getCanvasSize() {
        const view_size: any = {};
        view_size['transverse'] = [this.transverseCanvas.width, this.transverseCanvas.height];
        view_size['coronal'] = [this.coronalCanvas.width, this.coronalCanvas.height];
        view_size['saggital'] = [this.saggitalCanvas.width, this.saggitalCanvas.height];

        return view_size;
    }

    mainHideList() {
        document.getElementById('series_list').classList.toggle('series_inactive');
    }

    mainzoom() {
        this.picLeft1.addZoomEvent();
        this.picLeft2.addZoomEvent();
        this.picLeft3.addZoomEvent();
    }

    mainZoomPro(evt) {
        console.log(evt);
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.GetZoomPic(evt[0], evt[1]).subscribe(result => {
            result = JSON.parse(result);
            that.updateCells(result);
        });
    }

    mainWLWW() {
        this.picLeft1.addChangeWlEvent();
        this.picLeft2.addChangeWlEvent();
        this.picLeft3.addChangeWlEvent();
    }

    mainWWWLPro(evt) {
        console.log(evt);
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.GetWindowPic(evt[0], evt[1], evt[2]).subscribe(result => {
            result = JSON.parse(result);
            that.updateCells(result, true);
        });
    }

    mainWWWLPro2(evt) {
        const validFlag = evt[2];
        if (validFlag == 'true') {
            const that = this;
            if (this.hasLoadVolume) {
                this.seriesHttpService.GetWindowPic2(evt[0], evt[1]).subscribe(result => {
                    result = JSON.parse(result);
                    that.updateCells(result);
                });
            }
        } else {
            this.priMessageService.add({ severity: 'error', detail: 'ww wl illegal.' });
        }
    }

    mainpan() {
        this.picLeft1.addPanEvent();
        this.picLeft2.addPanEvent();
        this.picLeft3.addPanEvent();
    }

    mainPanPro(evt) {
        console.log(evt);
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.GetPanPic(evt[0], evt[1], evt[2]).subscribe(result => {
            result = JSON.parse(result);
            that.updateCells(result);
        });
    }

    mainrotate() {
        this.picLeft1.addRotateEvent();
        this.picLeft2.addRotateEvent();
        this.picLeft3.addRotateEvent();
    }

    mainRotatePro(evt) {
        console.log(evt);
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.GetRotatePic(evt[0], evt[1], evt[2]).subscribe(result => {
            result = JSON.parse(result);
            that.updateCells(result);
        });
    }

    mainSetCenterPro() {
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.GetCenterPic().subscribe(result => {
            result = JSON.parse(result);
            that.updateCells(result);
        });
    }

    mainreset() {
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.GetResetPic().subscribe(result => {
            result = JSON.parse(result);
            that.updateCells(result);
        });
    }

    sfile() {
        this.picLeft1.file();
    }

    remouse() {
        this.picLeft1.clearmouse();
        this.picLeft2.clearmouse();
        this.picLeft3.clearmouse();
    }

    getSeriesList(patientId: any) {
        this.seriesHttpService.getSeriesList(patientId).subscribe(data => {
            this.seriesList = data;
        })
    }

    loadSeries() {
        const transverseCanvas = $('.a_class .icanvas').get(0);
        const seriesId: any = $('#seriesSelect').val();
        const canvasSize: any = {};
        canvasSize['view_size'] = this.getCanvasSize();
        console.log(canvasSize);
        const that = this;
        if (seriesId === '' || seriesId == null || seriesId === undefined) {
            this.priMessageService.add({ severity: 'error', detail: 'No series selected.' });
            return;
        }
        this.priMessageService.add({ severity: 'info', detail: 'Loading now, please wait.' });
        this.seriesHttpService.LoadVolume(seriesId).subscribe(value => {
            value = JSON.parse(value);
            if (value.length === 3) {
                EventAggregator.Instance().volumnSize.publish(value);
                this.conService.noticeSize(canvasSize).subscribe(result => {
                    if (result.body === "success") {
                        this.seriesHttpService.GetSeries(seriesId, '', 'all', transverseCanvas.width, transverseCanvas.height)
                            .subscribe((value) => {
                                const data = JSON.parse(value);
                                that.updateCells(data, true);
                                this.sliceIndex = data['0']['slice_index'];
                                this.conMessage.SetSliceIndex(this.sliceIndex);
                                EventAggregator.Instance().sliceIndex.publish(this.sliceIndex);
                                this.priMessageService.add({ severity: 'success', detail: 'Load succeed.' });
                            }, (error) => {
                                this.priMessageService.add({ severity: 'error', detail: 'Load failed.' });
                                console.log(error);
                            });
                        this.hasLoadVolume = true;
                        this.seriesId = seriesId;
                    }
                });
            } else if (value === "rebuild") {
                this.priMessageService.add({ severity: 'error', detail: 'Load failed, rebuiding now, please wait' });
                this.seriesHttpService.ReLoadVolume(seriesId).subscribe(value => {
                    value = JSON.parse(value);
                    if (value.length === 3) {
                        this.conService.noticeSize(canvasSize).subscribe(result => {
                            if (result.body == "success") {
                                this.seriesHttpService.GetSeries(seriesId, '', 'all', transverseCanvas.width, transverseCanvas.height)
                                    .subscribe((value) => {
                                        const data = JSON.parse(value);
                                        that.updateCells(data, true);
                                        this.sliceIndex = data['0']['slice_index'];
                                        this.conMessage.SetSliceIndex(this.sliceIndex);
                                        EventAggregator.Instance().sliceIndex.publish(this.sliceIndex);
                                        this.priMessageService.add({ severity: 'success', detail: 'Load succeed.' });
                                    }, (error) => {
                                        this.priMessageService.add({ severity: 'error', detail: 'Load failed.' });
                                        console.log(error);
                                    });
                                this.hasLoadVolume = true;
                                this.seriesId = seriesId;
                            }
                        });
                    } else {
                        this.priMessageService.add({ severity: 'error', detail: `Rebuild failed. ${value}` });
                    }
                });
            } else {
                this.priMessageService.add({ severity: 'error', detail: 'Load failed.' });
            }
        });
    }

    fb(a) {
        this.load.loadbar(a);
    }

    message(w) {
        this.load.message(w);
    }

    saveContour(data: any) {
        if (data.length > 0) {
            console.log('begin save contour');

            const dto = new ContourDto();
            dto.roi_uid = data[0];
            dto.slice_index = data[1];
            dto.contours = data[2];

            this.conService.saveContour(dto).subscribe(response => {
                console.log(response);
            });
        }
    }

    deleteContours(data: any) {
        if (data.length > 0) {
            console.log('begin delete contour');

            const dto = new ContourDto();
            dto.roi_uid = data[0];
            dto.slice_index = data[1];

            this.conService.deleteContours(dto).subscribe(response => {
                console.log(response);
            });
        }
    }

    updateCells(data, updateWwwl: boolean = false, updateViews: string = 'all') {
        if (updateViews === 'all' || updateViews.indexOf('transverse') > -1) {
            this.picLeft1.cellUpdate(data['0']['image'], data['0']['crosshair'], data['0']['graphic']['contours'],
                updateWwwl ? data['0']['wwwl'] : undefined);
        }
        if (updateViews === 'all' || updateViews.indexOf('coronal') > -1) {
            this.picLeft2.cellUpdate(data['1']['image'], data['1']['crosshair'], data['1']['graphic']['contours'],
                updateWwwl ? data['1']['wwwl'] : undefined);
        }
        if (updateViews === 'all' || updateViews.indexOf('saggital') > -1) {
            this.picLeft3.cellUpdate(data['2']['image'], data['2']['crosshair'], data['2']['graphic']['contours'],
                updateWwwl ? data['2']['wwwl'] : undefined);
        }
    }

}
