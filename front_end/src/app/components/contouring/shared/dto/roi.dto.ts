import { IDto } from "./idto.dto";

export class RoiDto implements IDto {
    id;
    name;
    color;
    seriesuid;

    constructor(model = undefined) {
        if (model !== undefined) {
            for (var property in model) {
                if (model.hasOwnProperty(property))
                    (<any>this)[property] = (<any>model)[property];
            }
        }
    }

    serialize() {

    }

    deserialize(json) {
        this.id = json["id"];
        this.name = json["name"];
        this.color = json["color"];
        this.seriesuid = json["seriesUid"];
    }
}
