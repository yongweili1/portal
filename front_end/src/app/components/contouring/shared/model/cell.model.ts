import { Point } from "../tools/point";
import { KeyValuePair } from "../../../../shared/common/keyvaluepair";
declare var actions: any;

export class CellModel {
    tag: string;
    sliceIndex: number;
    imageM: ImageModel;
    crossM: CrossModel;
    graphics: any;
    actionInfo: any;

    constructor() {
        this.sliceIndex = 0;
        this.actionInfo = new KeyValuePair(actions.locate);
        this.imageM = new ImageModel();
        this.crossM = new CrossModel();
    }
}

export class ImageModel {
    imageData: string;
    windowLevel: any = 0;
    windowWidth: any = 2000;
}

export class CrossModel {
    point: Point;
    vColor: string;
    hColor: string;

    constructor() {
        this.point = new Point(0, 0);
    }
}

// export class OverlayModel {

// }
