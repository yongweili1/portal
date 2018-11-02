declare var delta_time: number;
declare var delta_count: number;

export class LazyExcuteHandler {
    recordTimeA: any;
    recordTimeB: any;
    recordTimeC: any;
    count = 0;

    constructor() {
        this.recordTimeA = 0;
        this.recordTimeB = 0;
        this.recordTimeC = 0;
    }

    canExcute(now, type='a') {
        let delta = 0
        if (type == 'a') {
            delta = now - this.recordTimeA;
            this.recordTimeA = now;
        } else if (type == 'b') {
            delta = now - this.recordTimeB;
            this.recordTimeB = now;
        } else if (type == 'c') {
            delta = now - this.recordTimeC;
            this.recordTimeC = now;
        } else {
            return true;
        }
        console.log(type, delta)
        return !(delta < delta_time);
    }

    canExcuteByCount() {
        this.count += 1
        if (this.count == delta_count) {
            this.count = 0;
            return true;
        }
        return false;
    }
}