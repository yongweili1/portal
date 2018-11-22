import { Component, OnInit, ViewChild } from '@angular/core';
import { ActivatedRoute } from '@angular/router';
import { TooltipService } from './shared/service/tooltip.service';
import { EventAggregator } from '../../shared/common/event_aggregator';
import { KeyValuePair } from '../../shared/common/keyvaluepair';
import { ExcuteHelper } from "./shared/tools/excute_helper";
import { ContourModel } from './shared/model/contour.model';
import { ContouringModel } from './shared/model/contouring.model';
import { RoiModel } from './shared/model/roi.model';
import { ConMessageService } from './shared/service/conMessage.service';
import { ContouringService } from './shared/service/contouring.service';
import { RoiHttpService } from './shared/service/roiHttp.service';
import { SeriesHttpService } from './shared/service/seriesHttp.service';
import { Point } from './shared/tools/point';

declare var $: any;
declare var actions: any;

@Component({
    selector: 'mpt-contouring',
    templateUrl: './contouring.component.html',
    styleUrls: ['./contouring.component.less'],
})
export class ContouringComponent implements OnInit {
    patientId: any = '';
    seriList: any;
    seriesList: any;
    hasLoadVolume = false;
    seriesId: any;

    newROIDisplay: any = false;
    manageROIDisplay: any = false;
    editROIDisplay: any = false;

    excuteHelper: ExcuteHelper;
    data: ContouringModel;

    @ViewChild('cell1') cell1;
    @ViewChild('cell2') cell2;
    @ViewChild('cell3') cell3;

    constructor(
        public activeRoute: ActivatedRoute,
        private conMessage: ConMessageService,
        public roiHttp: RoiHttpService,
        private seriesHttpService: SeriesHttpService,
        private conService: ContouringService,
        private tooltipService: TooltipService
    ) {
        this.excuteHelper = new ExcuteHelper();
        this.data = new ContouringModel();
        this.data.setTag();
        this.data.setCrossLineColor();
        EventAggregator.Instance().contourCps.subscribe(data => { this.saveContour(data); });
        EventAggregator.Instance().removeCps.subscribe(data => { this.deleteContours(data); });
    }

    //#region life-cycle hook methods
    ngOnInit() {
        this.cell1.id = 'cell-1';
        this.cell2.id = 'cell-2';
        this.cell3.id = 'cell-3';

        if (this.conMessage.seriList != undefined) {
            this.seriList = this.conMessage.seriList[0];
        }
        this.conMessage.seriList$.subscribe(value => {
            this.seriList = value;
        });

        EventAggregator.Instance().actionInfo.subscribe(value => {
            this.data.setActionInfo(value);
            const priActionArray = ['shape', 'clear', 'select', 'nudge'];
            let canvasType = '';
            if (value.key() === actions.locate) {
                canvasType = 'cross';
            } else if (priActionArray.indexOf(value.key()) > -1) {
                canvasType = 'overlay';
            } else {
                canvasType = 'action';
            }
            this.cell1.riseZIndexOfCanvas(canvasType);
            this.cell2.riseZIndexOfCanvas(canvasType);
            this.cell3.riseZIndexOfCanvas(canvasType);
        });

        this.activeRoute.queryParams.subscribe(params => {
            this.patientId = params.patientId;
        });

        const that = this;
        const canvasSize: any = {};
        $(window).resize(function () {
            setTimeout(() => {
                if (!that.hasLoadVolume) {
                    return;
                }
                canvasSize['view_size'] = that.getCanvasSize();
                that.conService.noticeSize(canvasSize).subscribe(result => {
                    if (result.body === "success" && that.hasLoadVolume) {
                        that.seriesHttpService.GetSeries("", "", "all", "", "").subscribe(data => {
                            data = JSON.parse(data);
                            that.updateCells(data);
                            this.updateSliceIndex(data['0']['slice_index']);
                        });
                    }
                });
            }, 300);
        });
    }

    ngAfterViewInit() {
        if (this.patientId === '' || this.patientId === undefined || this.patientId == null) {
            this.tooltipService.error('Please select the patient first.');
        } else {
            this.getSeriesList(this.patientId);
        }
    }

    ngOnDestroy() {
        this.hasLoadVolume = false;
        this.seriesHttpService.UnLoadVolume(this.seriesId).subscribe();
    }
    //#endregion

    //#region Roi
    handleAddRoi() {
        const seriesuid = $("#seriesSelect").val();
        if (seriesuid != '' && seriesuid !== undefined) {
            this.newROIDisplay = true;
            this.data.activeRoi = new RoiModel();
        } else {
            this.tooltipService.error('Please select series first.');
        }
    }

    handleEditRoi(roi: RoiModel) {
        this.editROIDisplay = true;
        this.data.activeRoi = roi;
    }

    handleDeleteRoi(roi: RoiModel) {
        this.roiHttp.DeleteROIConfig(roi.ROIId).subscribe(result => {
            if (result.code == '200') {
                this.tooltipService.success('Delete succeed.');
                this.data.roiList = result.data;
                this.editROIDisplay = false;
            } else {
                this.tooltipService.error('result.msg');
            }
        });
    }

    mainautoroi() {
        const ROIData = {
            seriesuid: $("#seriesSelect").val(),
            ROIName: 'heart',
            ROIColor: '#FFFF00'
        };
        this.roiHttp.CreateNewSegROI(ROIData).subscribe(result => {
            if (result.body.code == '200') {
                this.tooltipService.success('Save succeed.');
                this.data.roiList = result.body.data;
                this.newROIDisplay = false;
                // this.data.roiList.forEach(element => {
                //     // if (element.ROIId > 0) {
                //     //     this.activeROIConfig = element;
                //     // }
                // });

                // this.conMessage.SetActiveRoi(this.activeROIConfig);
            } else {
                this.tooltipService.error(result.msg);
            }
        })
    }

    handleManageRoi(showDialog = true) {
        let seriesuid = $("#seriesSelect").val();
        if (seriesuid != '' && seriesuid != null && seriesuid != undefined) {
            if (showDialog) {
                this.manageROIDisplay = true;
            }
            this.roiHttp.GetROIConfig(seriesuid).subscribe(result => {
                if (result.code = '200') {
                    this.data.roiList = result['data'];
                    if (this.data.roiList.length > 0) {
                        console.log('set default roi')
                        this.onSelectRoi(this.data.roiList[0]);
                    }
                }
            });
        } else {
            this.tooltipService.error('Please select series first.');
        }
    }

    hideNewROIDia() {
        this.data.activeRoi = new RoiModel();
    }

    saveROI() {
        if (this.data.activeRoi.ROIName == '') {
            this.tooltipService.error('Illegal input.');
            return;
        }
        this.roiHttp.PostCreateNewROI(this.data.activeRoi).subscribe(result => {
            if (result.body.code == '200') {
                this.tooltipService.success('Save succeed.');
                this.data.roiList = result.body.data;
                this.newROIDisplay = false;
                // this.data.roiList.forEach(element => {
                //     // if (element.ROIId > 0) {
                //     //     this.activeROIConfig = element;
                //     // }
                // });

                // this.conMessage.SetActiveRoi(this.activeROIConfig);
            } else {
                this.tooltipService.error(result.msg);
            }
        });
    }

    updateROI() {
        if (this.data.activeRoi.ROIName == '') {
            this.tooltipService.error('Illegal input.');
            return;
        }
        this.roiHttp.UpdateROIConfig(this.data.activeRoi).subscribe(result => {
            if (result.body.code == '200') {
                this.tooltipService.success('Save succeed.');
                this.data.roiList = result.body.data;
                this.editROIDisplay = false;
            } else {
                this.tooltipService.error(result.msg);
            }
        });
    }

    deleteAllROI() {
        const ROIIdArray = [];
        this.data.roiList.forEach(element => {
            ROIIdArray.push(element.ROIId);
        });
        this.roiHttp.DeleteROIConfig(ROIIdArray).subscribe(result => {
            if (result.code == '200') {
                this.tooltipService.success(`Delete succeed.`);
                this.data.roiList = result.data;
            } else {
                this.tooltipService.error(result.msg);
            }
        });
    }

    changeROIVisible(evt) {
        evt.target.classList.toggle("fa-eye-slash");
    }

    onSelectRoi(roi) {
        this.data.selectedRoi = roi;
        this.conMessage.SetActiveRoi(this.data.selectedRoi);
    }
    //#endregion

    //#region handle Locate event
    handleTransverseLocate(event: any) {
        if (!this.hasLoadVolume) {
            return;
        }
        const displayView = 'coronal,saggital';
        this.handleLocate('transverse', displayView, event);
    }

    handleCoronalLocate(event: any) {
        if (!this.hasLoadVolume) {
            return;
        }
        const displayView = 'transverse,saggital';
        this.handleLocate('coronal', displayView, event);
    }

    handleSaggitalLocate(event: any) {
        if (!this.hasLoadVolume) {
            return;
        }
        const displayView = 'transverse,coronal';
        this.handleLocate('saggital', displayView, event);
    }

    private handleLocate(focus: any, display: any, crossPoint: any) {
        this.seriesHttpService.GetLocatePic(focus, display, crossPoint).subscribe((value) => {
            const data = JSON.parse(value);
            this.updateCells(data, false, display);
            this.updateSliceIndex(data['0']['slice_index']);
        });
    }
    //#endregion

    handleClearGraphics() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.clear));
    }

    //#region handle Scroll event
    handleTransverseScroll(event: any) {
        if (!this.excuteHelper.canExcute(new Date().getTime(), 'a')
            || !this.hasLoadVolume) {
            return;
        }

        this.handleScroll('transverse', event);
    }

    handleCoronalScroll(event: any) {
        if (!this.excuteHelper.canExcute(new Date().getTime(), 'b') ||
            !this.hasLoadVolume) {
            return;
        }

        this.handleScroll('coronal', event);
    }

    handleSaggitalScroll(event: any) {
        if (!this.excuteHelper.canExcute(new Date().getTime(), 'c')
            || !this.hasLoadVolume) {
            return;
        }
        this.handleScroll('saggital', event);
    }

    private handleScroll(focus: any, delta: any) {
        this.seriesHttpService.GetSeriesPic(focus, focus, delta, '', '').subscribe((value) => {
            const data = JSON.parse(value);
            this.updateCells(data, false);
            this.updateSliceIndex(data['0']['slice_index']);
        }, (error) => {
            console.log(error);
        });
    }
    //#endregion

    getCanvasSize() {
        const view_size: any = {};
        view_size['transverse'] = [this.cell1.imageCanvas.width, this.cell1.imageCanvas.height];
        view_size['coronal'] = [this.cell2.imageCanvas.width, this.cell2.imageCanvas.height];
        view_size['saggital'] = [this.cell3.imageCanvas.width, this.cell3.imageCanvas.height];

        return view_size;
    }

    //#region handle Zoom event
    handleZoom(e) {
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.GetZoomPic(e[0], e[1]).subscribe(result => {
            result = JSON.parse(result);
            that.updateCells(result);
        });
    }
    //#endregion

    //#region handle Wwwl event
    handleWwwl(evt) {
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.GetWindowPic(evt[0], evt[1]).subscribe(result => {
            result = JSON.parse(result);
            that.updateCells(result, true);
        });
    }
    //#endregion

    //#region handle Pan event
    handlePan(e) {
        console.log(e);
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.GetPanPic(e[0], e[1], e[2]).subscribe(result => {
            result = JSON.parse(result);
            that.updateCells(result);
        });
    }
    //#endregion

    //#region handle Rotate event
    handleRotate(e) {
        console.log(e);
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.GetRotatePic(e[0], e[1], e[2]).subscribe(result => {
            result = JSON.parse(result);
            that.updateCells(result);
        });
    }
    //#endregion

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

    handleReset() {
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.GetResetPic().subscribe(result => {
            result = JSON.parse(result);
            that.updateCells(result);
        });
    }

    getSeriesList(patientId: any) {
        this.seriesHttpService.getSeriesList(patientId).subscribe(data => {
            this.seriesList = data;
        });
    }

    handleLoadSeries() {
        const seriesId: any = $('#seriesSelect').val();
        const canvasSize: any = {};
        canvasSize['view_size'] = this.getCanvasSize();
        console.log(canvasSize);
        const that = this;
        if (seriesId === '' || seriesId == null || seriesId === undefined) {
            this.tooltipService.error('No series selected.');
            return;
        }
        this.tooltipService.info('Loading now, please wait.');
        this.seriesHttpService.LoadVolume(seriesId).subscribe(value => {
            value = JSON.parse(value);
            if (value.length === 3) {
                this.handleManageRoi(false);
                EventAggregator.Instance().volumnSize.publish(value);
                this.conService.noticeSize(canvasSize).subscribe(result => {
                    if (result.body === "success") {
                        this.seriesHttpService.GetSeries(seriesId, '', 'all', this.cell1.imageCanvas.width, this.cell1.imageCanvas.height)
                            .subscribe((value) => {
                                const data = JSON.parse(value);
                                that.updateCells(data, true);
                                this.updateSliceIndex(data['0']['slice_index']);
                                this.tooltipService.success('Load succeed.');
                            }, (error) => {
                                this.tooltipService.error('Load failed.');
                                console.log(error);
                            });
                        this.hasLoadVolume = true;
                        this.seriesId = seriesId;
                    }
                });
            } else if (value === "rebuild") {
                this.tooltipService.error('Load failed, rebuiding now, please wait');
                this.seriesHttpService.ReLoadVolume(seriesId).subscribe(value => {
                    value = JSON.parse(value);
                    if (value.length === 3) {
                        this.handleManageRoi(false);
                        this.conService.noticeSize(canvasSize).subscribe(result => {
                            if (result.body == "success") {
                                this.seriesHttpService.GetSeries(seriesId, '', 'all', this.cell1.imageCanvas.width,
                                    this.cell1.imageCanvas.height).subscribe((value) => {
                                        const data = JSON.parse(value);
                                        that.updateCells(data, true);
                                        this.updateSliceIndex(data['0']['slice_index']);
                                        this.tooltipService.success('Load succeed.');
                                    }, (error) => {
                                        this.tooltipService.error('Load failed.');
                                        console.log(error);
                                    });
                                this.hasLoadVolume = true;
                                this.seriesId = seriesId;
                            }
                        });
                    } else {
                        this.tooltipService.error(`Rebuild failed. ${value}`);
                    }
                });
            } else {
                this.tooltipService.error('Load failed.');
            }
        });
    }

    saveContour(data: any) {
        if (data.length > 0) {
            console.log('begin save contour');

            const dto = new ContourModel();
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

            const dto = new ContourModel();
            dto.roi_uid = data[0];
            dto.slice_index = data[1];

            this.conService.deleteContours(dto).subscribe(response => {
                console.log(response);
            });
        }
    }

    private updateSliceIndex(index) {
        EventAggregator.Instance().sliceIndex.publish(index);
        this.data.setSliceIndex(index);
    }

    updateCells(data, updateWwwl: boolean = false, updateViews: string = 'all') {
        if (updateViews === 'all' || updateViews.indexOf('transverse') > -1) {
            this.data.cell1.imageM.imageData = data['0']['image'];
            this.data.cell1.crossM.point = new Point(data['0']['crosshair'][0], data['0']['crosshair'][1]);
            this.data.cell1.graphics = data['0']['graphic']['contours'];
            if (updateWwwl) {
                this.data.cell1.imageM.setWwwl(data['0']['wwwl']);
            }
        }
        if (updateViews === 'all' || updateViews.indexOf('coronal') > -1) {
            this.data.cell2.imageM.imageData = data['1']['image'];
            this.data.cell2.crossM.point = new Point(data['1']['crosshair'][0], data['1']['crosshair'][1]);
            this.data.cell2.graphics = data['1']['graphic']['contours'];
            if (updateWwwl) {
                this.data.cell2.imageM.setWwwl(data['1']['wwwl']);
            }
        }
        if (updateViews === 'all' || updateViews.indexOf('saggital') > -1) {
            this.data.cell3.imageM.imageData = data['2']['image'];
            this.data.cell3.crossM.point = new Point(data['2']['crosshair'][0], data['2']['crosshair'][1]);
            this.data.cell3.graphics = data['2']['graphic']['contours'];
            if (updateWwwl) {
                this.data.cell3.imageM.setWwwl(data['2']['wwwl']);
            }
        }
    }
}
