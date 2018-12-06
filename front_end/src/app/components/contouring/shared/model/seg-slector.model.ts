export interface Organ {
    name: string;
    code: number;
}
export class SegSelectorModel {
    orgens: Organ[];
    slectionOrgens: Organ[];
    constructor() {
        this.orgens = [
            { name: "heart", code: 1 },
            { name: "liver", code: 2 },
            { name: "spleen", code: 3 },
            { name: "lung", code: 4 },
            { name: "kidney", code: 5 },
        ];
        this.slectionOrgens = [
            { name: "heart", code: 1 },
        ]
    }


}