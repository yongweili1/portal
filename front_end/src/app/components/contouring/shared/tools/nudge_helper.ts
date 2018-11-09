import { ClipperHelper, ClipPoint } from '../tools/clipper_helper';
import { Point } from '../tools/point';
import { FaderContainer } from '../container/fader_container';

export class NudgeHelper {
    fader: FaderContainer;
    mode: string;

    constructor(fader) {
        this.fader = fader;
        this.mode = '';
    }

    SetMode(center: Point, voiPt: Array<Array<Point>>) {
        let voiPt_Clipper = this.convertPoints(voiPt);
        let pointInContour: boolean = this.ContourContainsPoint(voiPt_Clipper, { X: center.x, Y: center.y });
        let virtualFaderPt_Clipper = this.convertPoints([this.fader.getCps()])
        if (pointInContour) {
            let intersection = ClipperHelper.Clipper(voiPt_Clipper, virtualFaderPt_Clipper, 1, "intersection");
            if (intersection[0].length == this.fader.getCps().length) {
                this.mode = "CreateInFader";
            } else {
                this.mode = "InFader";
            }
        } else {
            let intersection = ClipperHelper.Clipper(voiPt_Clipper, virtualFaderPt_Clipper, 1, "intersection");
            if (intersection.length == 0) {
                this.mode = "CreateOutFader";
            } else {
                this.mode = "OutFader";
            }
        }
    } 

    private ContourContainsPoint(contours: Array<Array<ClipPoint>>, point: ClipPoint): boolean {
        let num: number = 0;
        for (var contour in contours) {
            let wn: number = 0, j: number = 0;
            for (let i = 0; i < contours[contour].length; i++) {
                if (i == contours[contour].length - 1) {
                    j = 0;
                } else {
                    j = j + 1;
                }

                if (contours[contour][i].Y <= point.Y) {   // 如果多边形的点 小于等于 选定点的 Y 坐标  
                    if (contours[contour][j].Y > point.Y) {  // 如果多边形的下一点 大于于 选定点的 Y 坐标  
                        if (this.IsLeft(contours[contour][i], contours[contour][j], point) > 0) {
                            wn++;
                        }
                    }
                } else {
                    if (contours[contour][j].Y <= point.Y) {
                        if (this.IsLeft(contours[contour][i], contours[contour][j], point) < 0) {
                            wn--;
                        }
                    }
                }
            }
            if (wn != 0) { num++; }
        }
        if (num % 2 == 0) {
            return false;
        } else {
            return true;
        }
    }

    private convertPoints(contours: Array<Array<Point>>) {
        let rois = new Array();
        contours.forEach(contour => {
            if (contour == null) return;
            let roi = new Array();
            contour.forEach(cp => {
                let p = new ClipPoint();
                p.X = cp.x;
                p.Y = cp.y;
                roi.push(p)
            });
            rois.push(roi);
        });
        return rois;
    }

    private IsLeft(P0: ClipPoint, P1: ClipPoint, P2: ClipPoint): number {
        let abc: number = ((P1.X - P0.X) * (P2.Y - P0.Y) - (P2.X - P0.X) * (P1.Y - P0.Y));
        return abc;
    }

    Push(contours) {
        if (this.fader.getCps().length == 0 || this.mode == null) {
            return null;
        }

        let voiPt_Clipper = this.convertPoints(contours);
        let pusherPt_Clipper = this.convertPoints([this.fader.getCps()]);
        if (this.mode == "InFader") {
            return this.InPusher(voiPt_Clipper, pusherPt_Clipper);
        } else if (this.mode == "OutFader") {
            return this.OutPusher(voiPt_Clipper, pusherPt_Clipper);
        } else if (this.mode == "CreateOutFader") {
            return this.OutPusher(voiPt_Clipper, pusherPt_Clipper);
        } else if (this.mode == "CreateInFader") {
            return this.InPusher(voiPt_Clipper, pusherPt_Clipper);
        }
    }

    private OutPusher(voiPt_Clipper: Array<Array<ClipPoint>>, pusherPt_Clipper: Array<Array<ClipPoint>>): Array<Array<number>> {
        let difference = [];
        difference = ClipperHelper.Clipper(voiPt_Clipper, pusherPt_Clipper, 1, "difference");
        return difference;
    }

    private InPusher(voiPt_Clipper: Array<Array<ClipPoint>>, pusherPt_Clipper: Array<Array<ClipPoint>>): Array<Array<number>> {
        let union = [];
        union = ClipperHelper.Clipper(voiPt_Clipper, pusherPt_Clipper, 1, "union");
        return union;
    }

    PointInPoly(pt, poly) {
        for (var c = false, i = -1, l = poly.length, j = l - 1; ++i < l; j = i)
            ((poly[i].y <= pt.y && pt.y < poly[j].y) || (poly[j].y <= pt.y && pt.y < poly[i].y))
            && (pt.x < (poly[j].x - poly[i].x) * (pt.y - poly[i].y) / (poly[j].y - poly[i].y) + poly[i].x)
            && (c = !c);
        return c;
    }
}