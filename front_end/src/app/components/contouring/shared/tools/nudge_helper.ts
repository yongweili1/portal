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

    getMode() {
        return this.mode;
    }

    setMode(center: Point, voiPt: Array<Array<Point>>) {
        const voiPt_Clipper = this.convertPoints(voiPt);
        const pointInContour: boolean = this.ContourContainsPoint(voiPt_Clipper, { X: center.x, Y: center.y });
        const virtualFaderPt_Clipper = this.convertPoints([this.fader.getCps()]);
        if (pointInContour) {
            const intersection = ClipperHelper.Clipper(voiPt_Clipper, virtualFaderPt_Clipper, 1, 'intersection');
            if (intersection[0].length === this.fader.getCps().length) {
                this.mode = 'CreateInFader';
            } else {
                this.mode = 'InFader';
            }
        } else {
            const intersection = ClipperHelper.Clipper(voiPt_Clipper, virtualFaderPt_Clipper, 1, 'intersection');
            if (intersection.length === 0) {
                this.mode = 'CreateOutFader';
            } else {
                this.mode = 'OutFader';
            }
        }
        console.log('fader mode', this.mode);
        return this.mode;
    }

    setState() {
        if (this.mode === 'CreateInFader' || this.mode === 'InFader') {
            this.fader.showState(1);
        } else if (this.mode === 'OutFader' || this.mode === 'CreateOutFader') {
            this.fader.showState(-1);
        }
    }

    private ContourContainsPoint(contours: Array<Array<ClipPoint>>, point: ClipPoint): boolean {
        let num = 0;
        for (const contour in contours) {
            if (contour === undefined || contour == null) {
                continue;
            }
            let wn = 0, j = 0;
            for (let i = 0; i < contours[contour].length; i++) {
                if (i === contours[contour].length - 1) {
                    j = 0;
                } else {
                    j = j + 1;
                }
                // 如果多边形的点 小于等于 选定点的 Y 坐标
                if (contours[contour][i].Y <= point.Y) {
                    // 如果多边形的下一点 大于于 选定点的 Y 坐标
                    if (contours[contour][j].Y > point.Y) {
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
            if (wn !== 0) { num++; }
        }
        if (num % 2 === 0) {
            return false;
        } else {
            return true;
        }
    }

    private convertPoints(contours: Array<Array<Point>>) {
        const rois = new Array();
        contours.forEach(contour => {
            if (contour == null) {
                return;
            }
            const roi = new Array();
            contour.forEach(cp => {
                const p = new ClipPoint();
                p.X = cp.x;
                p.Y = cp.y;
                roi.push(p);
            });
            rois.push(roi);
        });
        return rois;
    }

    private IsLeft(P0: ClipPoint, P1: ClipPoint, P2: ClipPoint): number {
        const abc: number = ((P1.X - P0.X) * (P2.Y - P0.Y) - (P2.X - P0.X) * (P1.Y - P0.Y));
        return abc;
    }

    Push(contours: Array<Array<Point>>, bridgeClipper: Array<Array<Point>>) {
        if (contours == null || this.fader.getCps().length === 0 || this.mode == null) {
            return [];
        }

        const voiPt_Clipper = this.convertPoints(contours);
        const pusherPt_Clipper = this.convertPoints([this.fader.getCps()]);
        const bridgePts = this.convertPoints(bridgeClipper);
        const clipper = this.InPush(bridgePts, pusherPt_Clipper);
        if (this.mode === 'InFader') { // push out
            return this.InPush(voiPt_Clipper, clipper);
        } else if (this.mode === 'OutFader') { // push in
            return this.OutPush(voiPt_Clipper, clipper);
        } else if (this.mode === 'CreateOutFader') { // create a new contour
            return this.OutPush(voiPt_Clipper, clipper);
        } else if (this.mode === 'CreateInFader') { // push out
            return this.InPush(voiPt_Clipper, clipper);
        }
    }

    private OutPush(voiPt_Clipper: Array<Array<ClipPoint>>, pusherPt_Clipper: any): Array<Array<number>> {
        let difference = [];
        difference = ClipperHelper.Clipper(voiPt_Clipper, pusherPt_Clipper, 1, 'difference');
        return difference;
    }

    private InPush(voiPt_Clipper: Array<Array<ClipPoint>>, pusherPt_Clipper: any): Array<Array<number>> {
        let union = [];
        union = ClipperHelper.Clipper(voiPt_Clipper, pusherPt_Clipper, 1, 'union');
        return union;
    }
}
