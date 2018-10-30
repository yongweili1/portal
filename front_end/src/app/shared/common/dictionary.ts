export class Dictionary {
    items: any = {};

    has(key) {
        return key in this.items;
    };

    set(key, value) {
        this.items[key] = value;
    };

    remove(key) {
        if (this.has(key)) {
            delete this.items[key];
            return true;
        }
        return false;
    };

    get(key) {
        return this.has(key) ? this.items[key] : undefined;
    };

    values() {
        var values = [];
        for (var k in this.items) {
            if (this.has(k)) {
                values.push(this.items[k]);
            }
        }
        return values;
    };

    clear() {
        this.items = {};
    };

    size() {
        var count = 0;
        for (var prop in this.items) {
            if (this.items.hasOwnProperty(prop)) {
                ++count;
            }
        }
        return count;
    };

    getItems = function () {
        return this.items;
    };
}
