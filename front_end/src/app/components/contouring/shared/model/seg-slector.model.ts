export interface Organ {
    // group: string;
    name: string;
    code: number;
}
export class SegSelectorModel {
    organs: Organ[][];
    slectionOrgans: Organ[];

    constructor() {
        this.organs = [
            [
                { name: "heart", code: 1 },
                { name: "liver", code: 2 },
                { name: "spleen", code: 3 },
            ],

            [
                { name: "lung", code: 4 },
                { name: "kidney", code: 5 },
                { name: "...", code: 6 }
            ],
            [
                { name: "..", code: 7 },
                { name: "..", code: 8 },
                { name: "..", code: 9 }
            ],
        ];
        this.slectionOrgans = [this.organs[0].find((organ: Organ) => organ.code == 1 && organ.name == "heart"),];
    }


}