import { Point, Point3d } from './point';

export class Utils {
    constructor() { }

    /**
     * @msg: 为每个Contour生成唯一的带时间戳的id
     * @return {string} contourUID
     */
    generateContourUid(): string {
        const org_root = '1.2.156.112605.22.';
        const dateStr = new Date().getTime();
        const randomNum: string = String(Math.floor(Math.random() * 1000));
        const res = org_root + dateStr + '.' + randomNum;
        return res;
    }

    /**
     * @msg: 计算两点的距离
     * @param {Point}} 类型两点 p1 p2
     * @return {number}
     */
    getLengthOfTwoPoint(p1: Point, p2: Point): number {
        return p1 !== undefined && p2 !== undefined ? Math.sqrt(Math.pow(p1.x - p2.x, 2) + Math.pow(p1.y - p2.y, 2)) : 0;
    }

    /**
     * @msg: 计算两点中心点
     * @param  {Point} 类型两点 p1 p2
     * @return {Point} 类型中心点
     */
    getCenterPoint(p1: Point, p2: Point): Point {
        return p1 !== undefined && p2 !== undefined ? new Point((p1.x + p2.x) / 2, (p1.y + p2.y) / 2) : undefined;
    }

    /**
     * @msg: 计算点到矩形四边的最小距离
     * @param {point}目标点point
     * @param {Array<point>}目标矩形
     * @return {number}
     */
    getMinLengthToRec(point: Point, rectangle: Array<Point>): number {
        if (rectangle !== undefined && rectangle.length >= 4) {
            const beginPointX = point.x;
            const beginPointY = point.y;
            const diams: number[] = [];
            diams[0] = beginPointX - rectangle[0].x;
            diams[2] = rectangle[1].x - beginPointX;
            diams[1] = beginPointY - rectangle[0].y;
            diams[3] = rectangle[2].y - beginPointY;
            let diam: number = diams[0];
            for (let index = 1; index < 4; index++) {
                if (diams[index] < diam) {
                    diam = diams[index];
                }
            }
            return diam;
        }
        return 0;
    }

    /**
     * @msg: 判断矩形是否在图像边界内
     * @param {startPoint} 图元矩形起始点
     * @param {endPoint} 图元矩形终止点
     * @return: 图元矩形是否在目标矩形内的布尔值
     */
    isRecInPolygon(startPoint: Point, endPoint: Point, polygonPoints: Array<Point>): boolean {
        const rectangle: Array<Point> = new Array<Point>();
        rectangle.push(startPoint);
        rectangle.push(endPoint);
        rectangle.push(new Point(startPoint.x, endPoint.y));
        rectangle.push(new Point(endPoint.x, startPoint.y));
        for (let index = 0; index < rectangle.length; index++) {
            if (!this.isInPolygon(rectangle[index], polygonPoints)) {
                return false;
            }
        }
        return true;
    }

    /**
     * 判断点是否在多边形内
     * @param checkPoint 被判断的点坐标
     * @param polygonPoints 多边形顶点坐标
     */
    isInPolygon(checkPoint: Point, polygonPoints: Array<Point>) {
        let counter = 0;
        let i;
        let xinters;
        let p1, p2;
        const pointCount = polygonPoints.length;
        p1 = polygonPoints[0];

        for (i = 1; i <= pointCount; i++) {
            p2 = polygonPoints[i % pointCount];
            if (
                checkPoint.x > Math.min(p1.x, p2.x) &&
                checkPoint.x <= Math.max(p1.x, p2.x)
            ) {
                if (checkPoint.y <= Math.max(p1.y, p2.y)) {
                    if (p1.x !== p2.x) {
                        xinters = (checkPoint.x - p1.x) * (p2.y - p1.y) / (p2.x - p1.x) + p1.y;
                        if (p1.y === p2.y || checkPoint.y <= xinters) {
                            counter++;
                        }
                    }
                }
            }
            p1 = p2;
        }
        if (counter % 2 === 0) {
            return false;
        } else {
            return true;
        }
    }

    /**
     * 获取两条直线的交点坐标
     * @param p0 第一条直线的第一个点
     * @param p1 第一条直线的第二个点
     * @param p2 第二条直线的第一个点
     * @param p3 第二条直线的第二个点
     */
    private getLineIntersection(p0: Point, p1: Point, p2: Point, p3: Point) {
        const s10_x = p1.x - p0.x;
        const s10_y = p1.y - p0.y;
        const s32_x = p3.x - p2.x;
        const s32_y = p3.y - p2.y;

        const denom = s10_x * s32_y - s32_x * s10_y;
        // 平行或共线
        if (denom === 0) {
            return null; // Collinear
        }
        const denomPositive = denom > 0;

        const s02_x = p0.x - p2.x;
        const s02_y = p0.y - p2.y;
        const s_numer = s10_x * s02_y - s10_y * s02_x;
        // 参数是大于等于0且小于等于1的，分子分母必须同号且分子小于等于分母
        if ((s_numer < 0) === denomPositive) {
            return null; // No collision
        }

        const t_numer = s32_x * s02_y - s32_y * s02_x;
        if ((t_numer < 0) === denomPositive) {
            return null; // No collision
        }

        if (Math.abs(s_numer) > Math.abs(denom) || Math.abs(t_numer) > Math.abs(denom)) {
            return null; // No collision
        }
        // Collision detected
        const t = t_numer / denom;

        const p = new Point();
        p.x = p0.x + (t * s10_x);
        p.y = p0.y + (t * s10_y);
        return p;
    }

    /**
     * 获取多边形和直线交点坐标，没有则返回null
     * @param shapeCps 多边形顶点坐标
     * @param lineCps 直线顶点坐标
     */
    getShapeIntersectionWithLine(shapeCps: Array<Point>, lineCps: Array<Point>) {
        if (shapeCps === undefined || shapeCps == null || shapeCps.length === 0) {
            return null;
        }
        if (lineCps === undefined || lineCps == null || lineCps.length !== 2) {
            return null;
        }
        for (let index = 0; index < shapeCps.length - 1; index++) {
            const startCp = shapeCps[index];
            const endCp = shapeCps[index + 1];
            const intersection = this.segmentsIntr(startCp, endCp, lineCps[0], lineCps[1]);
            if (intersection != null) {
                return intersection;
            }
        }
        return null;
    }

    /**
     * 获取两条直线的交点坐标
     * @param a 第一条直线的第一个点
     * @param b 第一条直线的第二个点
     * @param c 第二条直线的第一个点
     * @param d 第二条直线的第二个点
     */
    private segmentsIntr(a, b, c, d) {

        // 三角形abc 面积的2倍
        const area_abc = (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);

        // 三角形abd 面积的2倍
        const area_abd = (a.x - d.x) * (b.y - d.y) - (a.y - d.y) * (b.x - d.x);

        // 面积符号相同则两点在线段同侧,不相交 (对点在线段上的情况,本例当作不相交处理);
        if (area_abc * area_abd >= 0) {
            return null;
        }

        // 三角形cda 面积的2倍
        const area_cda = (c.x - a.x) * (d.y - a.y) - (c.y - a.y) * (d.x - a.x);
        // 三角形cdb 面积的2倍
        // 注意: 这里有一个小优化.不需要再用公式计算面积,而是通过已知的三个面积加减得出.
        const area_cdb = area_cda + area_abc - area_abd;
        if (area_cda * area_cdb >= 0) {
            return null;
        }

        // 计算交点坐标
        const t = area_cda / (area_abd - area_abc);
        const dx = t * (b.x - a.x),
            dy = t * (b.y - a.y);
        return new Point(a.x + dx, a.y + dy);
    }

    /**
     * 将矩形各边缩放delta的距离
     * @param pts 矩形顶点坐标
     * @param delta 各边缩放距离
     */
    scaleRectangleBoundary(pts, delta) {
        const center_x = (pts[0].x + pts[2].x) / 2;
        const center_y = (pts[0].y + pts[2].y) / 2;
        const center = [center_x, center_y];
        const pt1 = new Point3d(pts[0].x - center[0], pts[0].y - center[1], 1);
        const pt2 = new Point3d(pts[1].x - center[0], pts[1].y - center[1], 1);
        const pt3 = new Point3d(pts[2].x - center[0], pts[2].y - center[1], 1);
        const pt4 = new Point3d(pts[3].x - center[0], pts[3].y - center[1], 1);

        const width = Math.sqrt((pts[0].x - pts[1].x) ** 2 +
            (pts[0].y - pts[1].y) ** 2);
        const height = Math.sqrt((pts[0].x - pts[3].x) ** 2 +
            (pts[0].y - pts[3].y) ** 2);

        const sx = 2 * delta / width;
        const sy = 2 * delta / height;
        const matrix = [[sx, 0, 0], [0, sy, 0], [0, 0, 1]];

        const p11 = this._change(pt1, matrix);
        const p22 = this._change(pt2, matrix);
        const p33 = this._change(pt3, matrix);
        const p44 = this._change(pt4, matrix);
        const p1 = new Point(p11.x + center[0], p11.y + center[1]);
        const p2 = new Point(p22.x + center[0], p22.y + center[1]);
        const p3 = new Point(p33.x + center[0], p33.y + center[1]);
        const p4 = new Point(p44.x + center[0], p44.y + center[1]);
        return [p1, p2, p3, p4, p1.copy()];
    }

    private _change(p: Point3d, matrix: number[][]) {
        const pt = [p.x, p.y, p.z];
        const b = [p.x, p.y, p.z];
        for (let i = 0; i < 3; i++) {
            for (let j = 0; j < 3; j++) {
                b[i] += pt[j] * matrix[j][i];
            }
        }
        return new Point3d(b[0], b[1], b[2]);
    }

    /**
     * 将p点向相对点靠近0.001倍两点连线长度的距离
     * 该方法主要用于规避某些问题（当直线的某个端在在多边形上时，由于精度问题导致无法
     * 获得直线和多边形的交点，所以将p点向多边形中心点relatedP移动一点微小的距离），
     * 不具备普适性，请谨慎使用
     * @param p 被操作的点
     * @param relatedP 相对点
     */
    closeTo(p, relatedP) {
        const center = [relatedP.x, relatedP.y];
        const pt = new Point3d(p.x - center[0], p.y - center[1], 1);
        const matrix = [[-0.001, 0, 0], [0, -0.001, 0], [0, 0, 1]];

        const p11 = this._change(pt, matrix);
        const p1 = new Point(p11.x + center[0], p11.y + center[1]);
        return p1;
    }

    private _getNearestPt(p1: Point, p2: Point, p3: Point) {
        const P = new Point();

        // 如果p1.x==p2.x 说明是条竖着的线
        if (p1.x - p2.x === 0) {
            P.x = p1.x;
            P.y = p3.y;
        } else {
            const A = (p1.y - p2.y) / (p1.x - p2.x);
            const B = p1.y - A * p1.x;
            const m = p3.x + A * p3.y;

            P.x = (m - A * B) / (A * A + 1);
            P.y = A * P.x + B;
        }

        if ((P.x - p1.x) * (P.x - p2.x) <= 0 && (P.y - p1.y) * (P.y - p2.y) <= 0) {
            return P;
        }

        const dis1 = this.getLengthOfTwoPoint(p3, p1);
        const dis2 = this.getLengthOfTwoPoint(p3, p2);
        // const dis3 = this.getLengthOfTwoPoint(p3, P);
        if (dis1 < dis2) {
            return p1;
        } else {
            return p2;
        }
    }

    /**
     * 获取p点到多边形的最近点坐标
     * @param shapeCps 多边形顶点坐标
     * @param p 被操作的点
     */
    getNearestPt(shapeCps: Array<Point>, p: Point) {
        let nearestPt = new Point();
        for (let index = 0; index < shapeCps.length - 1; index++) {
            const startCp = shapeCps[index];
            const endCp = shapeCps[index + 1];
            const pt = this._getNearestPt(startCp, endCp, p);
            const dis1 = this.getLengthOfTwoPoint(p, pt);
            const dis2 = this.getLengthOfTwoPoint(p, nearestPt);
            if (dis1 < dis2) {
                nearestPt = pt;
            }
        }
        return nearestPt;
    }

    creatBridgeRectCps(prePoint: Point, currPoint: Point, radius: number) {
        const start = prePoint;
        const end = currPoint;
        if (start.equals(end)) {
            return null;
        }
        let vec = [end.x - start.x, end.y - start.y];
        const deno = Math.sqrt(vec[0] ** 2 + vec[1] ** 2);
        vec = [vec[0] / deno, vec[1] / deno];
        const crossVec = [-vec[1], vec[0]];

        // the rect
        const p1 = new Point(start.x + crossVec[0] * radius, start.y + crossVec[1] * radius);
        const p2 = new Point(end.x + crossVec[0] * radius, end.y + crossVec[1] * radius);
        const p3 = new Point(end.x - crossVec[0] * radius, end.y - crossVec[1] * radius);
        const p4 = new Point(start.x - crossVec[0] * radius, start.y - crossVec[1] * radius);
        return [p1, p2, p3, p4, p1.copy()];
    }
}
