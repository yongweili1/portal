export class RoiModel {
    id: string;
    name: string;
    color = '#FFFFFF';
    seriesuid: string;

    constructor(data = undefined) {
        if (data !== undefined) {
            for (const property in data) {
                if (data.hasOwnProperty(property)) {
                    (<any>this)[property] = (<any>data)[property];
                }
            }
        }
    }
}
