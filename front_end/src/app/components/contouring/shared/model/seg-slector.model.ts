export interface Orgen {
    name: string;
    code: number;
}
export class SegSelectorModel {
    orgens: Orgen[];
    slectionOrgens: Orgen[];
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