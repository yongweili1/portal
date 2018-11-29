import { AfterViewInit, Component, OnDestroy, OnInit, ViewChild } from '@angular/core';
import { ActivatedRoute } from '@angular/router';
import { EventAggregator } from '../../shared/common/event_aggregator';
import { KeyValuePair } from '../../shared/common/keyvaluepair';
import { ContourModel } from './shared/model/contour.model';
import { ContouringModel } from './shared/model/contouring.model';
import { RoiModel } from './shared/model/roi.model';
import { ContouringService } from './shared/service/contouring.service';
import { RoiHttpService } from './shared/service/roiHttp.service';
import { SeriesHttpService } from './shared/service/seriesHttp.service';
import { ToastService } from './shared/service/toast.service';
import { ExcuteHelper } from './shared/tools/excute_helper';
import { Point } from './shared/tools/point';

declare var $: any;
declare var actions: any;

@Component({
    selector: 'mpt-contouring',
    templateUrl: './contouring.component.html',
    styleUrls: ['./contouring.component.less'],
})
export class ContouringComponent implements OnInit, AfterViewInit, OnDestroy {
    patientId: any = '';
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
        public roiHttp: RoiHttpService,
        private seriesHttpService: SeriesHttpService,
        private conService: ContouringService,
        private toastService: ToastService
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
                that.updateCanvasSize(canvasSize);
            }, 200);
        });
    }

    ngAfterViewInit() {
        if (this.patientId === '' || this.patientId === undefined || this.patientId == null) {
            this.toastService.error('Please select the patient first.');
        } else {
            this.getSeriesList(this.patientId);
        }
    }

    ngOnDestroy() {
        this.hasLoadVolume = false;
        this.seriesHttpService.unloadVolume(this.seriesId).subscribe();
    }
    //#endregion

    //#region Roi
    handleAddRoi() {
        const seriesuid = $('#seriesSelect').val();
        if (seriesuid !== '' && seriesuid !== undefined) {
            this.newROIDisplay = true;
            this.data.activeRoi = new RoiModel();
        } else {
            this.toastService.error('Please select series first.');
        }
    }

    handleEditRoi(roi: RoiModel) {
        this.editROIDisplay = true;
        this.data.activeRoi = roi;
    }

    handleDeleteRoi(id: string) {
        this.roiHttp.delete(id).subscribe(result => {
            if (result.success) {
                this.toastService.success('Delete succeed.');
                const index = this.data.roiList.findIndex(x => x.id === id);
                this.data.roiList.splice(index, 1);
                EventAggregator.Instance().rois.publish(this.data.roiList);
            } else {
                this.toastService.error(result.msg);
            }
        });
    }

    mainautoroi() {
        const ROIData = {
            seriesuid: $('#seriesSelect').val(),
            name: 'heart',
            color: '#FFFF00'
        };
        this.roiHttp.CreateNewSegROI(ROIData).subscribe(result => {
            if (result.body.code == '200') {
                this.toastService.success('Save succeed.');
                this.data.roiList = result.body.data;
                EventAggregator.Instance().rois.publish(this.data.roiList);
                this.newROIDisplay = false;
            } else {
                this.toastService.error(result.msg);
            }
        });
    }

    handleManageRoi(showDialog = true) {
        const seriesuid = $('#seriesSelect').val();
        if (seriesuid !== '' && seriesuid !== undefined) {
            if (showDialog) {
                this.manageROIDisplay = true;
            }
            this.roiHttp.get(seriesuid).subscribe(response => {
                if (response.success) {
                    this.data.roiList = response.data;
                    EventAggregator.Instance().rois.publish(this.data.roiList);
                    if (this.data.roiList.length > 0) {
                        this.onSelectRoi(this.data.roiList[0]);
                    }
                } else {
                    this.toastService.error(response.message);
                }
            });
        } else {
            this.toastService.error('Please select series first.');
        }
    }

    hideNewROIDia() {
        this.data.activeRoi = new RoiModel();
    }

    saveROI() {
        this.data.activeRoi.seriesuid = $('#seriesSelect').val();
        this.roiHttp.create(this.data.activeRoi).subscribe(response => {
            response = response.body;
            if (response.success) {
                this.toastService.success('Save succeed.');
                this.data.activeRoi.id = response.data;
                this.data.roiList.push(this.data.activeRoi);
                EventAggregator.Instance().rois.publish(this.data.roiList);
                this.newROIDisplay = false;
            } else {
                this.toastService.success(response.message);
            }
        });
    }

    updateROI() {
        this.roiHttp.update(this.data.activeRoi).subscribe(result => {
            if (result.body.success) {
                this.toastService.success('Save succeed.');
                this.editROIDisplay = false;
            } else {
                this.toastService.error(result.body.message);
            }
        });
    }

    deleteAllROI() {
        const ROIIdArray = [];
        this.data.roiList.forEach(element => {
            ROIIdArray.push(element.id);
        });
        this.roiHttp.delete(ROIIdArray).subscribe(result => {
            if (result.code == '200') {
                this.toastService.success(`Delete succeed.`);
                this.data.roiList = result.data;
                EventAggregator.Instance().rois.publish(this.data.roiList);
            } else {
                this.toastService.error(result.msg);
            }
        });
    }

    changeROIVisible(evt) {
        evt.target.classList.toggle('fa-eye-slash');
    }

    onSelectRoi(roi) {
        this.data.selectedRoi = roi;
        EventAggregator.Instance().roi.publish(this.data.selectedRoi);
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
        this.seriesHttpService.locate(focus, crossPoint).subscribe((value) => {
            if (value.success) {
                const data = JSON.parse(value.data);
                this.updateCells(data, false, display);
                this.updateSliceIndex(data['0']['slice_index']);
            } else {
                this.toastService.error(value.message);
            }
        });
    }
    //#endregion

    //#region handle Clear graphics event
    handleClearGraphics() {
        EventAggregator.Instance().actionInfo.publish(new KeyValuePair(actions.clear));
    }
    //#endregion

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
        this.seriesHttpService.page(focus, delta).subscribe((value) => {
            if (value.success) {
                const data = JSON.parse(value.data);
                this.updateCells(data, false);
                this.updateSliceIndex(data['0']['slice_index']);
            } else {
                this.toastService.error(value.message);
            }
        }, (error) => {
            console.log(error);
        });
    }
    //#endregion

    private getCanvasSize() {
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
        this.seriesHttpService.zoom(e[0], e[1]).subscribe(result => {
            if (result.success) {
                result = JSON.parse(result.data);
                that.updateCells(result);
            } else {
                that.toastService.error(result.message);
            }
        });
    }
    //#endregion

    //#region handle Wwwl event
    handleWwwl(evt) {
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.wwwl(evt[0], evt[1]).subscribe(result => {
            if (result.success) {
                result = JSON.parse(result.data);
                that.updateCells(result);
            } else {
                that.toastService.error(result.message);
            }
        });
    }
    //#endregion

    //#region handle Pan event
    handlePan(e) {
        if (!this.hasLoadVolume) {
            return;
        }
        this.seriesHttpService.pan(e[0], e[1], e[2]).subscribe(result => {
            if (result.success) {
                result = JSON.parse(result.data);
                this.updateCells(result);
            } else {
                this.toastService.error(result.message);
            }
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
        this.seriesHttpService.rotate(e[0], e[1], e[2]).subscribe(result => {
            if (result.success) {
                result = JSON.parse(result.data);
                that.updateCells(result);
            } else {
                that.toastService.error(result.message);
            }
        });
    }
    //#endregion

    //#region handle Center event
    handleCenter() {
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.center().subscribe(result => {
            if (result.success) {
                result = JSON.parse(result.data);
                that.updateCells(result);
            } else {
                that.toastService.error(result.message);
            }
        });
    }
    //#endregion

    //#region handle Reset event
    handleReset() {
        if (!this.hasLoadVolume) {
            return;
        }
        const that = this;
        this.seriesHttpService.reset().subscribe(result => {
            if (result.success) {
                result = JSON.parse(result.data);
                that.updateCells(result);
            } else {
                that.toastService.error(result.message);
            }
        });
    }
    //#endregion

    private getSeriesList(patientId: any) {
        this.seriesHttpService.getSeriesList(patientId).subscribe(response => {
            if (response['success']) {
                this.seriesList = response['data'];
            } else {
                this.toastService.error(response['message']);
            }
        });
    }

    //#region handle Load Volume event
    handleLoadSeries() {
        const seriesId: any = $('#seriesSelect').val();
        const canvasSize: any = {};
        canvasSize['view_size'] = this.getCanvasSize();
        console.log(canvasSize);
        if (seriesId === '' || seriesId == null || seriesId === undefined) {
            this.toastService.error('No series selected.');
            return;
        }
        this.toastService.info('Loading now, please wait.');
        this.seriesHttpService.loadVolume(seriesId).subscribe(response => {
            if (response.success) {
                this.hasLoadVolume = true;
                this.seriesId = seriesId;
                this.handleManageRoi(false);
                EventAggregator.Instance().volumnSize.publish(response.data);
                this.updateCanvasSize(canvasSize);
            } else {
                this.toastService.error(response.message);
            }
        });
    }
    //#endregion

    private updateCanvasSize(canvasSize) {
        this.seriesHttpService.updateCanvasSize(canvasSize).subscribe(result => {
            if (result.success) {
                this.getImages();
            } else {
                this.toastService.error(result.message);
            }
        });
    }

    private getImages() {
        this.seriesHttpService.GetSeries().subscribe((value) => {
            if (value.success) {
                const data = JSON.parse(value.data);
                this.updateCells(data, true);
                this.updateSliceIndex(data['0']['slice_index']);
                this.toastService.success('succeed.');
            } else {
                this.toastService.error(value.message);
            }
        }, (error) => {
            this.toastService.error('failed.');
            console.log(error);
        });
    }

    private saveContour(data: any) {
        if (data.length > 0) {
            console.log('begin save contour');

            const dto = new ContourModel();
            dto.roi_uid = data[0];
            dto.slice_index = data[1];
            dto.contours = data[2];

            this.conService.save(dto).subscribe(response => {
                console.log(response);
            });
        }
    }

    private deleteContours(data: any) {
        if (data.length > 0) {
            console.log('begin delete contour');
            this.conService.delete(data[0], data[1]).subscribe(response => {
                console.log(response);
            });
        }
    }

    private updateSliceIndex(index) {
        EventAggregator.Instance().sliceIndex.publish(index);
        this.data.setSliceIndex(index);
    }

    private updateCells(data, updateWwwl: boolean = false, updateViews: string = 'all') {
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
