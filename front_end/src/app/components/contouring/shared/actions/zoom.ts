import { BaseAction } from './base';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
import { ActionTypeEnum } from '../../../../shared/models/enums';


export class Zoom extends BaseAction {
    zoom_factor = 0;
    preY = 0;

    handleMouseDown(e) {
        super.handleMouseDown(e);
        this.zoom_factor = 0;
        this.preY = e.clientY;
    }

    handleMouseMove(e) {
        if (!this.isMouseDown) {
            return;
        }
        const curY = e.clientY;
        const shiftY = curY - this.preY;
        if (shiftY === 0) {
            return;
        }
        this.preY = curY;
        if (shiftY >= 0) {
            this.zoom_factor = 1.0 + shiftY * 1.0 / 120;
        } else {
            this.zoom_factor = 1.0 / (1.0 - shiftY * 1.0 / 120);
        }
        const data = [];
        data[0] = ActionTypeEnum.zoom;
        data[1] = [this.tag, this.zoom_factor];
        EventAggregator.Instance().eventData.publish(data);
    }

    handleMouseUp(e) {
        super.handleMouseUp(e);
    }
}
