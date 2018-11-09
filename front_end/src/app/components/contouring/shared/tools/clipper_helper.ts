declare var ClipperLib: any

export abstract class ClipperHelper {
    constructor() { }

    public static Clipper(contours: Array<Array<ClipPoint>>, clipper: Array<Array<ClipPoint>>, scale: number = 1, clipperType: string): Array<Array<number>> {
        let returnValue = [];
        ClipperLib.JS.ScaleUpPaths(contours, scale);
        ClipperLib.JS.ScaleUpPaths(clipper, scale);
        let cpr = new ClipperLib.Clipper();
        cpr.AddPaths(contours, ClipperLib.PolyType.ptSubject, true);
        cpr.AddPaths(clipper, ClipperLib.PolyType.ptClip, true);
        let subject_fillType = ClipperLib.PolyFillType.pftNonZero;
        let clip_fillType = ClipperLib.PolyFillType.pftNonZero;

        if (clipperType == "difference") {
            cpr.Execute(ClipperLib.ClipType.ctDifference, returnValue, subject_fillType, clip_fillType);
            ClipperLib.JS.ScaleDownPaths(returnValue, scale);
            return returnValue;
        } else if (clipperType == "union") {
            cpr.Execute(ClipperLib.ClipType.ctUnion, returnValue, subject_fillType, clip_fillType);
            ClipperLib.JS.ScaleDownPaths(returnValue, scale);
            return returnValue;
        } else if (clipperType == "intersection") {
            cpr.Execute(ClipperLib.ClipType.ctIntersection, returnValue, subject_fillType, clip_fillType);
            ClipperLib.JS.ScaleDownPaths(returnValue, scale);
            return returnValue;
        } else if (clipperType == "xor") {
            cpr.Execute(ClipperLib.ClipType.ctXor, returnValue, subject_fillType, clip_fillType);
            ClipperLib.JS.ScaleDownPaths(returnValue, scale);
            return returnValue;
        } else {
            console.log("The clipper has not the " + clipperType);
            return null;
        }
    }
}

export class ClipPoint {
    public X: number;
    public Y: number;
}
