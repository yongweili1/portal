export class KeyValuePair {
    protected _key: string;
    protected _value: any;

    constructor(key, value = null) {
        this._key = key;
        this._value = value;
    }

    update(key, value) {
        this._key = key;
        this._value = value;
    }

    key() {
        return this._key;
    }

    value() {
        return this._value;
    }
}
