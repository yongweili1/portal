export class RoiModel {
    id: string;
    name: string;
    color: string;
    seriesuid: string;

    constructor(data = undefined) {
        if (data !== undefined) {
            for (var property in data) {
                if (data.hasOwnProperty(property))
                    (<any>this)[property] = (<any>data)[property];
            }
        }
    }
}
