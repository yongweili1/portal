import { Point } from '../tools/point'

export class ContourDto {    
    roi_uid: string;
    slice_index: string;
    contours: Array<Point>;
}