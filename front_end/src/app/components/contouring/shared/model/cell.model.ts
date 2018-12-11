import { Point } from '../tools/point';


export class CellModel {
    tag: string;
    sliceIndex: number;
    imageM: ImageModel;
    crossM: CrossModel;
    graphics: any;
    faderRadius: any;
    boundaryPts: any;

    constructor() {
        this.sliceIndex = 0;
        this.imageM = new ImageModel();
        this.crossM = new CrossModel();
        this.faderRadius = 20;
    }

    setBoundaryPts(pts) {
        const pts1 = [];
        pts.forEach(pt => {
            pts1.push(new Point(pt[0], pt[1]));
        });
        pts1.push(pts1[0].copy());
        this.boundaryPts = pts1;
    }
}

export class ImageModel {
    imageData: string;
    windowLevel: any = 0;
    windowWidth: any = 2000;

    setWwwl(wwwl) {
        if (wwwl === undefined || !Array.isArray(wwwl)) {
            return;
        }
        this.windowWidth = wwwl[0];
        this.windowLevel = wwwl[1];
    }
}

export class CrossModel {
    point: Point;
    vColor: string;
    hColor: string;

    constructor() {
        this.point = new Point(0, 0);
    }
}

