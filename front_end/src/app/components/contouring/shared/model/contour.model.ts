import { Point } from '../tools/point';

export class ContourModel {
    roi_uid: string;
    slice_index: string;
    contours: Array<Point>;
}
