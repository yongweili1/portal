export class ResponseDto<T> {
    success: boolean;
    message: string;
    data: T | any;

    constructor(response = undefined) {
        if (response !== undefined) {
            for (var property in response) {
                if (Array.isArray((<any>response)[property])) {
                    (<any>this)[property] = new Array<T>();
                    (<any>response)[property].forEach(element => {
                        (<any>this)[property].push(Parser.instance().parse<T>(element));
                    });
                } else {
                    (<any>this)[property] = (<any>response)[property];
                }
            }
        }
    }
}

export class Parser {
    private static _parser: Parser;

    private constructor() { }

    static instance() {
        if (this._parser === undefined) {
            this._parser = new Parser();
        }
        return this._parser;
    }

    parse<T>(response) {
        const obj = new Object() as T;
        if (response !== undefined) {
            for (var property in response) {
                if (response.hasOwnProperty(property)) {
                    (<T>obj)[property] = (<any>response)[property];
                }
            }
        }
        return obj;
    }
}
