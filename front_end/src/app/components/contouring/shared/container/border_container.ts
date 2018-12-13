import { BaseContainer } from '../container/base_container';
import { Freepen } from '../overlay/freepen';
import { RoiModel } from '../model/roi.model';
import { ShapeTypeEnum } from '../../../../shared/models/enums';

export class BorderContainer extends BaseContainer {
    shape: Freepen;

    constructor(stage) {
        super(stage, ShapeTypeEnum.border);
        this.shape = new Freepen(stage);
        this.initCps();
        this.addChild(this.shape);
    }

    public setRoi(roi: RoiModel) {
        super.setRoi(roi);
        this.shape.color = roi.color;
    }

    update() {
        super.update();
        this.shape.update(this.cps, this.isFill);
    }

    setCps(cps) {
        this.cps = cps;
    }
}
