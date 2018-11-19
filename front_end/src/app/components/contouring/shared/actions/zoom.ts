import { BaseAction } from "./base";
import { EventAggregator } from '../../../../shared/common/event_aggregator';
declare var actions: any;

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
        let curY = e.clientY;
        let shiftY = curY - this.preY;
        this.preY = curY;
        if (shiftY >= 0) {
            this.zoom_factor = 1.0 + shiftY * 1.0 / 120
        } else {
            this.zoom_factor = 1.0 / (1.0 - shiftY * 1.0 / 120)
        }
        const data = [];
        data[0] = actions.zoom;
        data[1] = [this.tag, this.zoom_factor];
        EventAggregator.Instance().eventData.publish(data);
    }

    handleMouseUp(e) {
        super.handleMouseUp(e);
    }
}
