import { AfterViewInit, Component, OnDestroy, OnInit, ViewChild } from '@angular/core';
import { ActivatedRoute } from '@angular/router';
import { EventAggregator } from '../../shared/common/event_aggregator';
import { ContourModel } from './shared/model/contour.model';
import { ContouringModel } from './shared/model/contouring.model';
import { RoiModel } from './shared/model/roi.model';
import { ContourService } from './shared/service/contour.service';
import { RoiService } from './shared/service/roi.service';
import { ImageService } from './shared/service/image.service';
import { ToastService } from './shared/service/toast.service';
import { ExcuteHelper } from './shared/tools/excute_helper';
import { Point } from './shared/tools/point';
import { Subscription } from 'rxjs';
import { ActionTypeEnum, CanvasTypeEnum } from '../../shared/models/enums';

declare var $: any;

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

    // subscription objects
    contourCpsSubscriber: Subscription;
    removeCpsSubscriber: Subscription;
    faderRadiusDeltaSubscriber: Subscription;
    actionTypeObserver: Subscription;
    shapeTypeObserver: Subscription;

    @ViewChild('cell1') cell1;
    @ViewChild('cell2') cell2;
    @ViewChild('cell3') cell3;

    constructor(
        public activeRoute: ActivatedRoute,
        private roiSvc: RoiService,
        private imageSvc: ImageService,
        private contourSvc: ContourService,
        private toastSvc: ToastService
    ) {
        this.excuteHelper = new ExcuteHelper();
        this.data = new ContouringModel();
        this.data.setTag();
        this.data.setCrossLineColor();
    }

    //#region life-cycle hook methods
    ngOnInit() {
        this.cell1.id = 'cell-1';
        this.cell2.id = 'cell-2';
        this.cell3.id = 'cell-3';

        this.contourCpsSubscriber = EventAggregator.Instance().contourCps
            .subscribe(data => {
                this.saveContours(data);
            });
        this.removeCpsSubscriber = EventAggregator.Instance().removeCps
            .subscribe(data => {
                this.deleteContours(data);
            });
        this.faderRadiusDeltaSubscriber = EventAggregator.Instance().faderRadiusDelta
            .subscribe(delta => {
                this.data.updateFaderRadius(delta);
            });

        this.actionTypeObserver = EventAggregator.Instance().changeActionTypeEvent
            .subscribe(action => {
                this.data.actionType = action;
                let canvasType = CanvasTypeEnum.unknown;
                if (action === ActionTypeEnum.locate) {
                    canvasType = CanvasTypeEnum.cross;
                } else if (action === ActionTypeEnum.nudge
                    || action === ActionTypeEnum.shape
                    || action === ActionTypeEnum.select) {
                    canvasType = CanvasTypeEnum.overlay;
                } else {
                    canvasType = CanvasTypeEnum.action;
                }
                this.cell1.riseZIndexOfCanvas(canvasType);
                this.cell2.riseZIndexOfCanvas(canvasType);
                this.cell3.riseZIndexOfCanvas(canvasType);
            });
        this.shapeTypeObserver = EventAggregator.Instance().changeShapeTypeEvent.subscribe(shape => {
            this.data.shapeType = shape;
            this.cell1.riseZIndexOfCanvas(CanvasTypeEnum.overlay);
            this.cell2.riseZIndexOfCanvas(CanvasTypeEnum.overlay);
            this.cell3.riseZIndexOfCanvas(CanvasTypeEnum.overlay);
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
        this.getSeriesList(this.patientId);
    }

    ngOnDestroy() {
        try {
            this.hasLoadVolume = false;
            this.imageSvc.unloadVolume(this.seriesId).subscribe();
            // 防止重复订阅
            this.contourCpsSubscriber.unsubscribe();
            this.removeCpsSubscriber.unsubscribe();
            this.faderRadiusDeltaSubscriber.unsubscribe();
            this.actionTypeObserver.unsubscribe();
            this.shapeTypeObserver.unsubscribe();
        } catch (error) {
            console.error(error.message);
        }
    }
    //#endregion

    //#region Roi
    handleAddRoi() {
        const seriesuid = $('#seriesSelect').val();
        if (seriesuid !== '' && seriesuid !== undefined) {
            this.newROIDisplay = true;
            this.data.activeRoi = new RoiModel();
        } else {
            this.toastSvc.error('Please select series first.');
        }
    }

    handleEditRoi(roi: RoiModel) {
        this.editROIDisplay = true;
        this.data.activeRoi = roi;
    }

    handleDeleteRoi(id: string) {
        this.roiSvc.delete(id).subscribe(response => {
            if (response.success) {
                this.toastSvc.success('Delete succeed.');
                const rois = [];
                this.data.roiList.forEach(roi => {
                    if (roi.id === id) {
                        return;
                    }
                    rois.push(roi);
                });
                this.data.roiList = rois;
                if (this.data.roiList !== null && this.data.roiList.length > 0) {
                    this.onSelectRoi(this.data.roiList[0]);
                }
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }

    mainautoroi() {
        const ROIData = {
            seriesuid: $('#seriesSelect').val(),
            name: 'heart',
            color: '#FFFF00'
        };
        this.roiSvc.CreateNewSegROI(ROIData).subscribe(response => {
            if (response.success) {
                this.toastSvc.success('Save succeed.');
                this.data.roiList = response.data;
                this.newROIDisplay = false;
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }

    handleManageRoi(showDialog = true) {
        const seriesuid = $('#seriesSelect').val();
        if (seriesuid !== '' && seriesuid !== undefined) {
            if (showDialog) {
                this.manageROIDisplay = true;
            }
            this.roiSvc.get(seriesuid).subscribe(response => {
                if (response.success) {
                    this.data.roiList = response.data;
                    if (this.data.roiList.length > 0) {
                        this.onSelectRoi(this.data.roiList[0]);
                    }
                } else {
                    this.toastSvc.error(response.message);
                }
            });
        } else {
            this.toastSvc.error('Please select series first.');
        }
    }

    hideNewROIDia() {
        this.data.activeRoi = new RoiModel();
    }

    saveROI() {
        this.data.activeRoi.seriesuid = $('#seriesSelect').val();
        this.roiSvc.create(this.data.activeRoi).subscribe(response => {
            if (response.success) {
                this.toastSvc.success('Save succeed.');
                this.data.activeRoi.id = response.data;
                const rois = [];
                rois.push(this.data.activeRoi);
                this.data.roiList.forEach(roi => {
                    rois.push(roi);
                });
                this.data.roiList = rois;
                this.onSelectRoi(this.data.roiList[0]);
                this.newROIDisplay = false;
            } else {
                this.toastSvc.success(response.message);
            }
        });
    }

    updateROI() {
        this.roiSvc.update(this.data.activeRoi).subscribe(response => {
            if (response.success) {
                this.toastSvc.success('Save succeed.');
                this.editROIDisplay = false;
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }

    deleteAllROI() {
        const ids = [];
        this.data.roiList.forEach(element => {
            ids.push(element.id);
        });
        this.roiSvc.delete(ids).subscribe(response => {
            if (response.success) {
                this.toastSvc.success(`Delete all rois succeed.`);
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }

    changeROIVisible(evt) {
        evt.target.classList.toggle('fa-eye-slash');
    }

    onSelectRoi(roi) {
        this.data.selectedRoi = roi;
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
        this.imageSvc.locate(focus, crossPoint).subscribe(response => {
            if (response.success) {
                this.updateCells(response.data, false, display);
                this.updateSliceIndex(response.data[0].slice_index);
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }
    //#endregion

    //#region handle Clear graphics event
    handleClearGraphics() {
        EventAggregator.Instance().clearGraphicsEvent.publish(undefined);
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
        this.imageSvc.page(focus, delta).subscribe(response => {
            if (response.success) {
                this.updateCells(response.data, false);
                this.updateSliceIndex(response.data[0].slice_index);
                this.getContours(response.data[0].slice_index);
            } else {
                this.toastSvc.error(response.message);
            }
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
        this.imageSvc.zoom(e[0], e[1]).subscribe(response => {
            if (response.success) {
                this.getContours(response.data[0].slice_index);
                that.updateCells(response.data);
            } else {
                that.toastSvc.error(response.message);
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
        this.imageSvc.wwwl(evt[0], evt[1]).subscribe(response => {
            if (response.success) {
                that.updateCells(response.data);
            } else {
                that.toastSvc.error(response.message);
            }
        });
    }
    //#endregion

    //#region handle Pan event
    handlePan(e) {
        if (!this.hasLoadVolume) {
            return;
        }
        this.imageSvc.pan(e[0], e[1], e[2]).subscribe(response => {
            if (response.success) {
                this.getContours(response.data[0].slice_index);
                this.updateCells(response.data);
            } else {
                this.toastSvc.error(response.message);
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
        this.imageSvc.rotate(e[0], e[1], e[2]).subscribe(response => {
            if (response.success) {
                this.getContours(response.data[0].slice_index);
                that.updateCells(response.data);
            } else {
                that.toastSvc.error(response.message);
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
        this.imageSvc.center().subscribe(response => {
            if (response.success) {
                this.getContours(response.data[0].slice_index);
                that.updateCells(response.data);
            } else {
                that.toastSvc.error(response.message);
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
        this.imageSvc.reset().subscribe(response => {
            if (response.success) {
                this.getContours(response.data[0].slice_index);
                that.updateCells(response.data);
            } else {
                that.toastSvc.error(response.message);
            }
        });
    }
    //#endregion

    private getSeriesList(patientId: any) {
        if (this.patientId === '' || this.patientId === undefined) {
            this.toastSvc.error('Please select patient firstly.');
        } else {
            this.imageSvc.getSeriesList(patientId).subscribe(response => {
                if (response.success) {
                    this.seriesList = response.data;
                } else {
                    this.toastSvc.error(response.message);
                }
            });
        }
    }

    //#region handle Load volume event
    handleLoadVolume() {
        const seriesId: any = $('#seriesSelect').val();
        const canvasSize: any = {};
        canvasSize['view_size'] = this.getCanvasSize();
        console.log(canvasSize);
        if (seriesId === '' || seriesId === undefined) {
            this.toastSvc.error('No series selected.');
            return;
        }
        this.toastSvc.info('Loading now, please wait.');
        this.imageSvc.loadVolume(seriesId).subscribe(response => {
            if (response.success) {
                this.hasLoadVolume = true;
                this.seriesId = seriesId;
                this.handleManageRoi(false);
                EventAggregator.Instance().volumnSize.publish(response.data);
                this.updateCanvasSize(canvasSize);
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }
    //#endregion

    private updateCanvasSize(canvasSize) {
        this.imageSvc.updateCanvasSize(canvasSize).subscribe(response => {
            if (response.success) {
                this.getImages();
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }

    private getImages() {
        this.imageSvc.center().subscribe(response => {
            if (response.success) {
                this.updateCells(response.data, true);
                this.updateSliceIndex(response.data[0].slice_index);
                this.getContours(response.data[0].slice_index);
                this.toastSvc.success('succeed.');
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }

    //#region contours
    private getContours(img_uid: string) {
        const roi_uids = new Array();
        this.data.roiList.forEach(roi => {
            roi_uids.push(roi.id);
        });
        this.contourSvc.getContours(roi_uids, img_uid).subscribe(response => {
            if (response.success) {
                this.data.cell1.graphics = response.data;
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }

    private saveContours(data: any) {
        if (data === undefined || data.length === 0) {
            return;
        }
        const dto = new ContourModel();
        dto.roi_uid = data[0];
        dto.slice_index = data[1];
        dto.contours = data[2];
        this.contourSvc.save(dto).subscribe(response => {
            if (response.success) {
                this.toastSvc.success('Success.');
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }

    private deleteContours(data: any) {
        if (data === undefined || data.length === 0) {
            return;
        }
        this.contourSvc.delete(data[0], data[1]).subscribe(response => {
            if (response.success) {
                this.toastSvc.success('Success.');
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }
    //#endregion

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
