import { BaseAction } from './base';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
import { ActionTypeEnum } from '../../../../shared/models/enums';


export class Pan extends BaseAction {
    prePos: any;

    handleMouseDown(e) {
        super.handleMouseDown(e);
        this.prePos = [e.clientX, e.clientY];
    }

    handleMouseMove(e) {
        if (!this.isMouseDown) {
            return;
        }
        if (e.clientX === this.prePos[0] && e.clientY === this.prePos[1]) {
            return;
        }
        const curPos = [e.clientX, e.clientY];
        const data = [];
        data[0] = ActionTypeEnum.pan;
        data[1] = [this.tag, this.prePos, curPos];
        EventAggregator.Instance().eventData.publish(data);
        this.prePos = curPos;
    }

    handleMouseUp(e) {
        super.handleMouseUp(e);
    }
}
