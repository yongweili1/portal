import { BaseAction } from './base';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
declare var actions: any;

export class Rotate extends BaseAction {
    prePos: any;

    handleMouseDown(e) {
        super.handleMouseDown(e);
        this.prePos = [e.clientX, e.clientY];
    }

    handleMouseMove(e) {
        if (!this.isMouseDown) {
            return;
        }
        const curPos = [e.clientX, e.clientY];
        const data = [];
        data[0] = actions.rotate;
        data[1] = [this.tag, this.prePos, curPos];
        EventAggregator.Instance().eventData.publish(data);
        this.prePos = curPos;
    }

    handleMouseUp(e) {
        super.handleMouseUp(e);
    }
}
