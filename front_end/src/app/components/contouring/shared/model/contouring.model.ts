import { CellModel } from './cell.model';
import { RoiModel } from './roi.model';

export class ContouringModel {
    cell1: CellModel;
    cell2: CellModel;
    cell3: CellModel;
    roiList: Array<RoiModel>;
    selectedRoi: RoiModel;
    activeRoi: RoiModel;

    constructor() {
        this.cell1 = new CellModel();
        this.cell2 = new CellModel();
        this.cell3 = new CellModel();
        this.roiList = new Array<RoiModel>();
        this.selectedRoi = new RoiModel();
        this.activeRoi = new RoiModel();
    }

    setTag() {
        this.cell1.tag = 'transverse';
        this.cell2.tag = 'coronal';
        this.cell3.tag = 'saggital';
    }

    setSliceIndex(index) {
        this.cell1.sliceIndex = index;
        this.cell2.sliceIndex = index;
        this.cell3.sliceIndex = index;
    }

    setCrossLineColor() {
        this.cell1.crossM.hColor = '#2196F3';
        this.cell1.crossM.vColor = '#CDDC39';
        this.cell2.crossM.hColor = '#F44336';
        this.cell2.crossM.vColor = '#CDDC39';
        this.cell3.crossM.hColor = '#F44336';
        this.cell3.crossM.vColor = '#2196F3';
    }

    setActionInfo(value) {
        this.cell1.actionInfo = value;
        this.cell2.actionInfo = value;
        this.cell3.actionInfo = value;
    }

    updateFaderRadius(delta) {
        this.cell1.faderRadius += delta;
        this.cell2.faderRadius += delta;
        this.cell3.faderRadius += delta;
    }
}
