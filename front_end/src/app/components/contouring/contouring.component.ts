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
import { SegSelectorModel } from './shared/model/seg-slector.model';


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
    segDisplay: any = false;
    manageROIDisplay: any = false;
    editROIDisplay: any = false;
    fillGraphic = true;

    excuteHelper: ExcuteHelper;
    data: ContouringModel;
    segData: SegSelectorModel;
    // subscription objects
    contourCpsSubscriber: Subscription;
    removeCpsSubscriber: Subscription;
    updateCpsSubscriber: Subscription;
    saveSingleCpsSubscriber: Subscription;
    removeSingleCpsSubscriber: Subscription;
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
        private toastSvc: ToastService,
    ) {
        this.excuteHelper = new ExcuteHelper();
        this.data = new ContouringModel();
        this.data.setTag();
        this.data.setCrossLineColor();

        this.segData = new SegSelectorModel();
    }

    //#region life-cycle hook methods
    ngOnInit() {
        this.cell1.id = 'cell-1';
        this.cell2.id = 'cell-2';
        this.cell3.id = 'cell-3';


        this.contourCpsSubscriber = EventAggregator.Instance().saveContoursEvent
            .subscribe(data => {
                this.saveContours(data);
            });
        this.removeCpsSubscriber = EventAggregator.Instance().deleteContoursEvent
            .subscribe(data => {
                this.deleteContours(data);
            });
        this.updateCpsSubscriber = EventAggregator.Instance().updateSigleContourEvent
            .subscribe(data => {
                this.updateSingleContour(data);
            });
        this.saveSingleCpsSubscriber = EventAggregator.Instance().saveSigleContourEvent
            .subscribe(data => {
                this.saveSingleContour(data);
            });
        this.removeSingleCpsSubscriber = EventAggregator.Instance().deleteSigleContourEvent
            .subscribe(data => {
                // this.deleteContours(data);
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
        this.shapeTypeObserver = EventAggregator.Instance().changeShapeTypeEvent
            .subscribe(shape => {
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
            this.updateCpsSubscriber.unsubscribe();
            this.saveSingleCpsSubscriber.unsubscribe();
            this.removeSingleCpsSubscriber.unsubscribe();
        } catch (error) {
            console.error(error.message);
        }
    }
    //#endregion

    //#region Roi

    disableContextMenu(evt) {
        evt.preventDefault();
        /*
            Implementation of custom menu
        */
    }

    handleAddRoi() {
        const seriesuid = $('#seriesSelect').val();
        if (seriesuid !== undefined && seriesuid != null && seriesuid !== '') {
            this.newROIDisplay = true;
            this.data.activeRoi = new RoiModel();
        } else {
            this.toastSvc.error('Please select series first.');
        }
    }

    handleSeg() {
        const ROIData = {
            seriesuid: $('#seriesSelect').val(),
            organs: this.segData.slectionOrgans
        };
        if (ROIData.seriesuid === undefined || ROIData.seriesuid === '') {
            this.toastSvc.error('Please select a valid series !');
            return;
        }
        this.roiSvc.startSeg(ROIData).subscribe(response => {
            if (response.success) {
                this.toastSvc.success('Save succeed.');
                this.data.roiList = response.data;
                this.onSelectRoi(this.data.selectedRoi);
                this.getContours(this.data.cell1.sliceIndex.toString());
                this.segDisplay = false;
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }

    handleContourToMask() {
        console.log('contour to mask');
        const seriesuid = $('#seriesSelect').val();
        const roiuid = this.data.selectedRoi.id;
        this.roiSvc.contourToMask(seriesuid, roiuid).subscribe(response => {
            if (response.success) {
                this.toastSvc.success('Save contours to mask succeed.');
            } else {
                this.toastSvc.error(response.message);
            }
        });
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
                this.onSelectRoi(this.data.selectedRoi);
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }

    mainautoroi() {
        this.segDisplay = true;
    }

    manageRoiDisplay() {
        const seriesuid = $('#seriesSelect').val();
        if (seriesuid !== undefined && seriesuid !== null && seriesuid !== '') {
            this.manageROIDisplay = !this.manageROIDisplay;
        }
    }

    handleManageRoi() {
        const seriesuid = $('#seriesSelect').val();
        if (seriesuid !== undefined && seriesuid !== null && seriesuid !== '') {
            this.roiSvc.get(seriesuid).subscribe(response => {
                if (response.success) {
                    this.data.roiList = response.data;
                    this.onSelectRoi(this.data.selectedRoi);
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
                this.onSelectRoi(this.data.selectedRoi);
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
                this.data.roiList = [];
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
        if (this.data.roiList !== undefined && this.data.roiList !== null && this.data.roiList.length > 0) {
            if (this.data.roiList.filter((_roi) => _roi.id === roi.id).length > 0) {
                this.data.selectedRoi = roi;
            } else {
                this.data.selectedRoi = this.data.roiList[0];
            }
        }
    }

    fillGraphicChanged(value) {
        this.data.updateFillGraphic(value);
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

    handleSlideChangeEnd(event: any) {
        this.handleScroll('transverse', event);
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
                this.updateBoundaryPts(response.data);
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
                this.getContours(response.data[0].slice_index);
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
                this.updateBoundaryPts(response.data);
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
                this.updateBoundaryPts(response.data);
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
                this.updateBoundaryPts(response.data);
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
                this.updateBoundaryPts(response.data);
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
                this.handleManageRoi();
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
                this.updateBoundaryPts(response.data);
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
        console.log('saveContours');
        if (data === undefined || data.length === 0) {
            return;
        }
        const dto = new ContourModel();
        dto.contours = data[0];
        dto.contour_type = data[1];
        dto.roi_uid = data[2];
        dto.slice_index = data[3];
        this.contourSvc.save(dto).subscribe(response => {
            if (response.success) {
                this.toastSvc.success('Success.');
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }

    private updateSingleContour(data: any) {
        console.log('updateSingleContour');
        const dto = new ContourModel();
        [dto.contours, dto.contour_uid] = data;
        this.contourSvc.update(dto).subscribe(response => {
            if (response.success) {
                this.toastSvc.success('Success.');
            } else {
                this.toastSvc.error(response.message);
            }
        });
    }

    private saveSingleContour(data: any) {
        console.log('saveSingleContour');
        if (data === undefined || data.length === 0) {
            return;
        }
        if (data[5] === 'transverse') {
            console.log('save contour start !');
            const dto = new ContourModel();
            [dto.contours, dto.contour_type, dto.contour_uid, dto.roi_uid, dto.slice_index] = data;
            this.contourSvc.save(dto).subscribe(response => {
                if (response.success) {
                    this.toastSvc.success('Success.');
                } else {
                    this.toastSvc.error(response.message);
                }
            });
        }
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

    private updateBoundaryPts(data) {
        this.data.cell1.setBoundaryPts(data[0].boundary_pts);
        this.data.cell2.setBoundaryPts(data[1].boundary_pts);
        this.data.cell3.setBoundaryPts(data[2].boundary_pts);
    }

    private updateCells(data, updateWwwl: boolean = false, updateViews: string = 'all') {
        if (updateViews === 'all' || updateViews.indexOf('transverse') > -1) {
            this.data.cell1.imageM.imageData = data['0']['image'];
            this.data.cell1.crossM.point = new Point(data['0']['crosshair'][0], data['0']['crosshair'][1]);
            if (updateWwwl) {
                this.data.cell1.imageM.setWwwl(data['0']['wwwl']);
            }
        }
        if (updateViews === 'all' || updateViews.indexOf('coronal') > -1) {
            this.data.cell2.imageM.imageData = data['1']['image'];
            this.data.cell2.crossM.point = new Point(data['1']['crosshair'][0], data['1']['crosshair'][1]);
            if (updateWwwl) {
                this.data.cell2.imageM.setWwwl(data['1']['wwwl']);
            }
        }
        if (updateViews === 'all' || updateViews.indexOf('saggital') > -1) {
            this.data.cell3.imageM.imageData = data['2']['image'];
            this.data.cell3.crossM.point = new Point(data['2']['crosshair'][0], data['2']['crosshair'][1]);
            if (updateWwwl) {
                this.data.cell3.imageM.setWwwl(data['2']['wwwl']);
            }
        }
    }
}
