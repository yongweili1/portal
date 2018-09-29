import { Pipe, PipeTransform } from '@angular/core';
import * as moment from 'moment';

@Pipe({
    name: 'duration'
})
export class DurationPipe implements PipeTransform {
    public transform(startTime: any, endTime: any): any {
        if (!startTime || !endTime) {
            return '-';
        }

        let span = moment(endTime).diff(moment(startTime));
        return this.duration(span);
    }

    private duration(span: number): string {
        if (span < 1000) {
            return span + 'ms';
        }

        let d = moment.duration(span);
        if (span < 60000) {
            return d.seconds() + 's';
        }
        return d.humanize();
    }
}
