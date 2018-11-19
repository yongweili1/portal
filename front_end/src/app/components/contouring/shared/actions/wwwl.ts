import { BaseAction } from "./base";
import { EventAggregator } from '../../../../shared/common/event_aggregator';
declare var actions: any;

export class WWWL extends BaseAction {
    ww_factor = 1;
    wl_factor = 1;
    preX = 0;
    preY = 0;

    handleMouseDown(e) {
        super.handleMouseDown(e);
        this.ww_factor = 1;
        this.wl_factor = 1;
        this.preX = e.clientX;
        this.preY = e.clientY;
    }

    handleMouseMove(e) {
        if (!this.isMouseDown) {
            return;
        }
        let curX = e.clientX;
        let curY = e.clientY;
        let shiftY = curY - this.preY;
        if (shiftY >= 0) {
            this.ww_factor = 1.0 + shiftY * 1.0 / 1000
        } else {
            this.ww_factor = 1.0 / (1.0 - shiftY * 1.0 / 1000)
        }
        this.wl_factor = (this.preX - curX) * 1.0 / 2000;
        this.preX = curX;
        this.preY = curY;

        const data = [];
        data[0] = actions.window;
        data[1] = [this.ww_factor, this.wl_factor];
        EventAggregator.Instance().eventData.publish(data);
    }

    handleMouseUp(e) {
        super.handleMouseUp(e);
    }
}
